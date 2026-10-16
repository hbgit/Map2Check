/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#include "NonDetPass.hpp"

#include <string>
#include <utility>
#include <vector>

#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

using llvm::dyn_cast;
using llvm::Instruction;
using llvm::IRBuilder;

namespace {

/** Names the benchmarks use for "prune this path", implemented as an abort.
 *
 * `__VERIFIER_assume` is usually only declared, and the call-site rewriting
 * below handles that. These are DEFINED in the program -- the SV-COMP sources
 * carry their own body:
 *
 *     void assume_abort_if_not(int cond) { if (!cond) abort(); }
 *
 * so renaming them would collide with the runtime's own definition. The body
 * is replaced instead, which reaches every call site at once. */
const char *const kAssumeByAbort[] = {"assume_abort_if_not",
                                      "assume_abort_if_not_",
                                      "__VERIFIER_assume",
                                      // The XCSP family spells it this way,
                                      // and spelling was the whole difference:
                                      // 59 of 59 XCSP tasks answered
                                      // "program correct" in two seconds
                                      // because the first failing assumption
                                      // aborted the run, and every one of
                                      // those programs has a reachable bug.
                                      // Same defect as the name above, missed
                                      // for being one word shorter.
                                      "assume"};

/** Rewrites such a function to a real assume. Returns true if it rewrote.
 *
 * Aborting on a failed assumption is not merely slow, it is wrong twice over.
 * KLEE runs here with --exit-on-error-type=Abort, so the FIRST path that
 * violates an assumption halts the whole search -- discarding every path not
 * yet explored. And the abort leaves an abort.err behind that looks exactly
 * like a real violation, which is how a path the competition considers out of
 * scope came back as a test case: one measured suite carried 25 inputs and
 * covered 0.0%.
 *
 * Measured over the 818-task Cover-Error corpus, the categories saturated with
 * this idiom are the ones with no recall at all -- Sequentialized 86% of tasks
 * and 0 confirmed, Floats 82% and 0, Arrays 71% and 1. */
bool rewriteAssumeToPrune(Function &F) {
  if (F.isDeclaration()) return false;
  bool named = false;
  for (const char *candidate : kAssumeByAbort) {
    if (F.getName() == candidate) {
      named = true;
      break;
    }
  }
  if (!named) return false;
  // The signature check carries more weight now that a name as generic as
  // "assume" is on the list: void(int) is the assumption idiom's shape, and a
  // function called assume with any other shape is something else and is left
  // alone.
  if (F.arg_size() != 1 || !F.getArg(0)->getType()->isIntegerTy()) return false;
  if (!F.getReturnType()->isVoidTy()) return false;

  // The program's own __VERIFIER_assume gives way to the library's: its
  // body would otherwise call itself, and the link would see two
  // definitions.
  llvm::FunctionCallee assume =
      Map2Check::getOrInsertRuntime(*F.getParent(),
                                    Map2Check::RuntimeHook::Assume);
  if (assume.getCallee() == &F) {
    F.deleteBody();
    Map2Check::allowUnwind(F);
    llvm::errs() << "[map2check] dropped the program's " << F.getName()
                 << " for the runtime's\n";
    return true;
  }

  LLVMContext &ctx = F.getContext();
  llvm::Value *condition = F.getArg(0);

  F.deleteBody();
  BasicBlock *entry = BasicBlock::Create(ctx, "entry", &F);
  IRBuilder<> builder(entry);
  // C reads the int condition as cond != 0.
  llvm::Value *holds = builder.CreateICmpNE(
      condition, llvm::Constant::getNullValue(condition->getType()));
  builder.CreateCall(assume, {builder.CreateZExt(holds, builder.getInt32Ty())});
  builder.CreateRetVoid();
  // The assume unwinds through this function now.
  Map2Check::allowUnwind(F);
  Map2Check::allowUnwind(*llvm::cast<Function>(assume.getCallee()));
  llvm::errs() << "[map2check] rewrote " << F.getName()
               << " to prune the path instead of aborting\n";
  return true;
}

}  // namespace

namespace {

/// The C type suffixes of the __VERIFIER_nondet_<t> generators the pass logs
/// (abi-mapping.tsv, family map2check_nondet_<t>).
const char *const kNonDetTypes[] = {
    "int",   "unsigned", "char",   "pointer", "long",     "ushort",
    "short", "ulong",    "bool",   "uchar",   "pchar",    "size_t",
    "loff_t", "sector_t", "uint",  "double"};

/// The C type of a __VERIFIER_nondet_<t> callee, or "" when it is not one.
llvm::StringRef nonDetType(const Function &callee) {
  llvm::StringRef name = callee.getName();
  if (!name.consume_front("__VERIFIER_nondet_")) return "";
  for (const char *type : kNonDetTypes) {
    if (name == type) return type;
  }
  return "";
}

/// Converts the generator's value to the C type of the logger's slot, or
/// returns nullptr when the program declared the generator with a type that
/// cannot be converted.
Value *toSlotType(IRBuilder<> &builder, Value *value,
                  const Map2Check::NonDetValueSlot &slot) {
  llvm::Type *from = value->getType();
  llvm::Type *to = slot.type;
  if (from == to) return value;
  if (from->isIntegerTy() && to->isIntegerTy()) {
    // An i1 (_Bool) is 0 or 1 whatever the slot's signedness.
    bool isSigned = slot.isSigned && !from->isIntegerTy(1);
    return builder.CreateIntCast(value, to, isSigned);
  }
  if (from->isPointerTy() && to->isIntegerTy()) {
    return builder.CreatePtrToInt(value, to);
  }
  if (from->isIntegerTy() && to->isPointerTy()) {
    return builder.CreateIntToPtr(value, to);
  }
  if (from->isFloatingPointTy() && to->isFloatingPointTy()) {
    return builder.CreateFPCast(value, to);
  }
  return nullptr;
}

}  // namespace

PreservedAnalyses NonDetPass::run(Function &F,
                                  llvm::FunctionAnalysisManager &AM) {
  // The library's assume rejects an input by unwinding through its callers,
  // so no declaration of it or call to it may be nounwind (D1 condition 11).
  if (Function *assume = F.getParent()->getFunction(
          Map2Check::runtimeCall(Map2Check::RuntimeHook::Assume,
                                 F.getContext())
              .name)) {
    Map2Check::allowUnwind(*assume);
  }

  // Before anything else: this replaces the whole body, so instrumenting the
  // old one first would be work thrown away.
  if (rewriteAssumeToPrune(F)) {
    return PreservedAnalyses::none();
  }
  if (F.isDeclaration()) return PreservedAnalyses::none();

  // Collect first, instrument after: the instrumentation inserts and removes
  // instructions.
  std::vector<std::pair<CallInst *, llvm::StringRef>> nonDetCalls;
  std::vector<CallInst *> crabAssumes;
  for (BasicBlock &bb : F) {
    for (Instruction &i : bb) {
      auto *callInst = dyn_cast<CallInst>(&i);
      if (callInst == nullptr) continue;
      auto *callee = dyn_cast<Function>(
          callInst->getCalledOperand()->stripPointerCasts());
      if (callee == nullptr) continue;
      llvm::StringRef type = nonDetType(*callee);
      if (!type.empty()) {
        nonDetCalls.emplace_back(callInst, type);
      } else if (callee->getName() == "verifier.assume") {
        crabAssumes.push_back(callInst);
      }
    }
  }
  if (nonDetCalls.empty() && crabAssumes.empty()) {
    return PreservedAnalyses::all();
  }

  IRBuilder<> entryBuilder(&*F.getEntryBlock().getFirstInsertionPt());
  this->functionName = entryBuilder.CreateGlobalStringPtr(F.getName());
  for (auto &[callInst, type] : nonDetCalls) {
    this->instrumentNonDet(callInst, type);
  }
  for (CallInst *callInst : crabAssumes) {
    this->instrumentCrabAssume(callInst);
  }
  return PreservedAnalyses::none();
}

// abi-mapping: map2check_non_det_<t> -> __VERIFIER_nondet_<t>,
// map2check_nondet_<t> -> map2check_save_nondet_log_<t>.
void NonDetPass::instrumentNonDet(CallInst *callInst, llvm::StringRef type) {
  Function &F = *callInst->getFunction();
  llvm::Module &module = *F.getParent();
  LLVMContext &ctx = F.getContext();

  // The generator keeps its name, except __VERIFIER_nondet_unsigned, which
  // the library spells __VERIFIER_nondet_uint. Only this call is retargeted:
  // the other calls keep naming their own C type, so they are logged as it.
  std::string generator = Map2Check::nonDetGenerator(type);
  if (callInst->getCalledOperand()->stripPointerCasts()->getName() !=
      generator) {
    llvm::FunctionCallee core = module.getOrInsertFunction(
        generator, callInst->getFunctionType());
    callInst->setCalledFunction(callInst->getFunctionType(), core.getCallee());
  }

  Map2Check::NonDetValueSlot slot = Map2Check::nonDetValueSlot(type, ctx);
  if (slot.type == nullptr || callInst->getType()->isVoidTy()) return;

  // The logger takes the value by address, in its C type
  // (docs/api-deltas.md of the library), so it lives in an entry alloca.
  // map2check.runtime marks the slot as instrumentation, which
  // MemoryTrackPass does not track as program memory.
  llvm::MDNode *runtimeTag = llvm::MDNode::get(ctx, {});
  IRBuilder<> entryBuilder(&*F.getEntryBlock().getFirstInsertionPt());
  llvm::AllocaInst *valueSlot = entryBuilder.CreateAlloca(
      slot.type, nullptr, "map2check.nondet." + type);
  valueSlot->setMetadata(Map2Check::kRuntimeMetadata, runtimeTag);

  IRBuilder<> builder(callInst->getNextNode());
  Value *value = toSlotType(builder, callInst, slot);
  if (value == nullptr) return;
  llvm::StoreInst *store = builder.CreateStore(value, valueSlot);
  store->setMetadata(Map2Check::kRuntimeMetadata, runtimeTag);

  DebugInfo debugInfo(&ctx, callInst);
  llvm::FunctionCallee logger = module.getOrInsertFunction(
      Map2Check::nonDetLogger(type), Map2Check::nonDetLoggerType(ctx));
  Value *args[] = {debugInfo.getLineNumberValue(),
                   debugInfo.getScopeNumberValue(),
                   llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx),
                                          slot.typeVar),
                   valueSlot, this->functionName};
  builder.CreateCall(logger, args);
}

// abi-mapping: map2check_crab_assume -> __VERIFIER_assume. Clam's
// verifier.assume(i1) is nounwind; the core assume takes an i32 and unwinds,
// so the call is replaced instead of renamed.
void NonDetPass::instrumentCrabAssume(CallInst *callInst) {
  llvm::Module &module = *callInst->getModule();
  llvm::FunctionCallee assume =
      Map2Check::getOrInsertRuntime(module, Map2Check::RuntimeHook::Assume);
  IRBuilder<> builder(callInst);
  Value *condition = callInst->getArgOperand(0);
  if (!condition->getType()->isIntegerTy(1)) {
    condition = builder.CreateICmpNE(
        condition, llvm::Constant::getNullValue(condition->getType()));
  }
  CallInst *replacement = builder.CreateCall(
      assume, {builder.CreateZExt(condition, builder.getInt32Ty())});
  replacement->setDebugLoc(callInst->getDebugLoc());
  callInst->eraseFromParent();
  Map2Check::allowUnwind(*llvm::cast<Function>(assume.getCallee()));
}

// --- New Pass Manager plugin registration ---
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "NonDetPass", LLVM_VERSION_STRING,
          [](llvm::PassBuilder& PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager& FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "nondet-pass") {
                    FPM.addPass(NonDetPass());
                    return true;
                  }
                  return false;
                });
          }};
}
