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

#include <memory>

#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

using llvm::CastInst;
using llvm::dyn_cast;
using llvm::IRBuilder;
using std::make_unique;
using llvm::Twine;

namespace {
inline Instruction *BBIteratorToInst(BasicBlock::iterator i) {
  Instruction *pointer = reinterpret_cast<Instruction *>(&*i);
  return pointer;
}
}  // namespace

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
                                      "__VERIFIER_assume"};

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
  if (F.arg_size() != 1 || !F.getArg(0)->getType()->isIntegerTy()) return false;
  if (!F.getReturnType()->isVoidTy()) return false;

  LLVMContext &ctx = F.getContext();
  llvm::Type *int32 = llvm::Type::getInt32Ty(ctx);
  llvm::Value *condition = F.getArg(0);

  F.deleteBody();
  BasicBlock *entry = BasicBlock::Create(ctx, "entry", &F);
  IRBuilder<> builder(entry);
  llvm::FunctionCallee assume = F.getParent()->getOrInsertFunction(
      "map2check_assume", llvm::Type::getVoidTy(ctx), int32);
  builder.CreateCall(assume, {builder.CreateSExtOrTrunc(condition, int32)});
  builder.CreateRetVoid();
  llvm::errs() << "[map2check] rewrote " << F.getName()
               << " to prune the path instead of aborting\n";
  return true;
}

}  // namespace

PreservedAnalyses NonDetPass::run(Function &F,
                                  llvm::FunctionAnalysisManager &AM) {
  // Before anything else: this replaces the whole body, so instrumenting the
  // old one first would be work thrown away.
  if (rewriteAssumeToPrune(F)) {
    return PreservedAnalyses::none();
  }

  this->nonDetFunctions = make_unique<NonDetFunctions>(&F, &F.getContext());
  bool initializedFunctionName = false;
  for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
    for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {
      if (!initializedFunctionName) {
        IRBuilder<> builder(BBIteratorToInst(i));
        this->functionName = builder.CreateGlobalStringPtr(F.getName());
        initializedFunctionName = true;
      }
      if (CallInst *callInst = dyn_cast<CallInst>(&*i)) {
        currentInstruction = i;
        this->runOnCallInstruction(callInst, &F.getContext());
      }
    }
  }
  return PreservedAnalyses::none();
}

namespace {
#define CALL_IF_HELPER(type, nondet_type, c_type)                             \
  if ((calleeFunction->getName() == ("__VERIFIER_nondet_" #c_type))) {        \
    this->instrumentNonDet(NonDetType::nondet_type, calleeFunction);          \
    this->instrumentNonDet##type(callInst, Ctx);                              \
  } else if ((calleeFunction->getName() == ("map2check_non_det_" #c_type))) { \
    this->instrumentNonDet##type(callInst, Ctx);                              \
  }
}  // namespace

void NonDetPass::runOnCallInstruction(CallInst *callInst, LLVMContext *Ctx) {
  Function *calleeFunction = callInst->getCalledFunction();

  if (calleeFunction == NULL) {
    Value *v = callInst->getCalledOperand();
    calleeFunction = dyn_cast<Function>(v->stripPointerCasts());
    if (calleeFunction == NULL) {
      return;
    }
  }

  // TODO(rafa.sa.xp@gmail.com): Should not rename __VERIFIER functions
  // clang-format
  if (true) {
    CALL_IF_HELPER(Integer, INTEGER, int)
  }
  CALL_IF_HELPER(Unsigned, UNSIGNED, unsigned)
  CALL_IF_HELPER(Char, CHAR, char)
  CALL_IF_HELPER(Pointer, POINTER, pointer)
  CALL_IF_HELPER(Long, LONG, long)
  CALL_IF_HELPER(Ushort, USHORT, ushort)
  CALL_IF_HELPER(Short, SHORT, short)
  CALL_IF_HELPER(Ulong, ULONG, ulong)
  CALL_IF_HELPER(Bool, BOOL, bool)
  CALL_IF_HELPER(Uchar, UCHAR, uchar)
  CALL_IF_HELPER(Pchar, PCHAR, pchar)
  CALL_IF_HELPER(Size_t, SIZE_T, size_t)
  CALL_IF_HELPER(Loff_t, LOFF_T, loff_t)
  CALL_IF_HELPER(Sector_t, SECTOR_T, sector_t)
  CALL_IF_HELPER(Uint, UINT, uint)
  CALL_IF_HELPER(Double, DOUBLE, double)

  if ((calleeFunction->getName() == "__VERIFIER_assume")) {
    this->instrumentNonDet(NonDetType::ASSUME, calleeFunction);
  } else if ((calleeFunction->getName() == "verifier.assume")) {
    this->instrumentNonDet(NonDetType::CRAB_ASSUME, calleeFunction);
  } else if ((calleeFunction->getName() == "map2check_assume")) {
    // TODO(rafa.sa.xp@gmail.com): Implement method
  }
  // clang-format on
}

namespace {
#define INSTRUMENT_CASE_HELPER(nondet_type, c_type) \
  case (NonDetType::nondet_type): {                 \
    Twine non_det("map2check_non_det_" #c_type);    \
    calleeFunction->setName(non_det);               \
    break;                                          \
  }
}  // namespace
void NonDetPass::instrumentNonDet(NonDetType nonDetType,
                                  Function *calleeFunction) {
  switch (nonDetType) {
    INSTRUMENT_CASE_HELPER(INTEGER, int)
    INSTRUMENT_CASE_HELPER(UNSIGNED, unsigned)
    INSTRUMENT_CASE_HELPER(CHAR, char)
    INSTRUMENT_CASE_HELPER(POINTER, pointer)
    INSTRUMENT_CASE_HELPER(LONG, long)
    INSTRUMENT_CASE_HELPER(USHORT, ushort)
    INSTRUMENT_CASE_HELPER(SHORT, short)
    INSTRUMENT_CASE_HELPER(ULONG, ulong)
    INSTRUMENT_CASE_HELPER(BOOL, bool)
    INSTRUMENT_CASE_HELPER(UCHAR, uchar)
    INSTRUMENT_CASE_HELPER(PCHAR, pchar)
    INSTRUMENT_CASE_HELPER(SIZE_T, size_t)
    INSTRUMENT_CASE_HELPER(LOFF_T, loff_t)
    INSTRUMENT_CASE_HELPER(SECTOR_T, sector_t)
    INSTRUMENT_CASE_HELPER(UINT, uint)
    INSTRUMENT_CASE_HELPER(DOUBLE, double)
    case (NonDetType::ASSUME): {
      Twine assume("map2check_assume");
      calleeFunction->setName(assume);
      break;
    }
    case (NonDetType::CRAB_ASSUME): {
      Twine assume("map2check_crab_assume");
      calleeFunction->setName(assume);
      break;
    }
  }
}

namespace {
#define NONDET_IMPL_HELPER_POINTER(type)                                    \
  void NonDetPass::instrumentNonDet##type(CallInst *callInst,               \
                                          LLVMContext *Ctx) {               \
    auto j = this->currentInstruction;                                      \
    j++;                                                                    \
    IRBuilder<> builder(BBIteratorToInst(j));                               \
    Value *function_llvm = this->getFunctionNameValue();                    \
    Value *cast = builder.CreatePtrToInt(callInst, Type::getInt32Ty(*Ctx)); \
    DebugInfo debugInfo(Ctx, callInst);                                     \
    Value *args[] = {debugInfo.getLineNumberValue(),                        \
                     debugInfo.getScopeNumberValue(), cast, function_llvm}; \
    FunctionCallee NonDetFunction =                                              \
        this->nonDetFunctions->getNonDet##type##Function();                 \
    builder.CreateCall(NonDetFunction, args);                               \
  }

#define NONDET_IMPL_HELPER_CAST(type)                                        \
  void NonDetPass::instrumentNonDet##type(CallInst *callInst,                \
                                          LLVMContext *Ctx) {                \
    auto j = this->currentInstruction;                                       \
    j++;                                                                     \
    IRBuilder<> builder(BBIteratorToInst(j));                                \
    Value *function_llvm = this->getFunctionNameValue();                     \
    Twine bitcast("map2check_nondet_cast_" #type);                           \
    Value *castInteger =                                                     \
        CastInst::CreateIntegerCast(callInst, Type::getInt32Ty(*Ctx), false, \
                                    bitcast, BBIteratorToInst(j));           \
    DebugInfo debugInfo(Ctx, callInst);                                      \
    Value *args[] = {debugInfo.getLineNumberValue(),                         \
                     debugInfo.getScopeNumberValue(), castInteger,           \
                     function_llvm};                                         \
    FunctionCallee NonDetFunction =                                               \
        this->nonDetFunctions->getNonDet##type##Function();                  \
    builder.CreateCall(NonDetFunction, args);                                \
  }

#define NONDET_IMPL_HELPER(type)                                \
  void NonDetPass::instrumentNonDet##type(CallInst *callInst,   \
                                          LLVMContext *Ctx) {   \
    auto j = this->currentInstruction;                          \
    j++;                                                        \
    IRBuilder<> builder(BBIteratorToInst(j));                   \
    Value *function_llvm = this->getFunctionNameValue();        \
    DebugInfo debugInfo(Ctx, callInst);                         \
    Value *args[] = {debugInfo.getLineNumberValue(),            \
                     debugInfo.getScopeNumberValue(), callInst, \
                     function_llvm};                            \
    FunctionCallee NonDetFunction =                                  \
        this->nonDetFunctions->getNonDet##type##Function();     \
    builder.CreateCall(NonDetFunction, args);                   \
  }
}  // namespace

NONDET_IMPL_HELPER(Integer)
NONDET_IMPL_HELPER(Unsigned)
NONDET_IMPL_HELPER(Uint)
NONDET_IMPL_HELPER(Double)
NONDET_IMPL_HELPER_CAST(Char)
NONDET_IMPL_HELPER_CAST(Ushort)
NONDET_IMPL_HELPER_CAST(Short)
NONDET_IMPL_HELPER_CAST(Long)
NONDET_IMPL_HELPER_CAST(Ulong)
NONDET_IMPL_HELPER_CAST(Bool)
NONDET_IMPL_HELPER_CAST(Uchar)
NONDET_IMPL_HELPER_CAST(Size_t)
NONDET_IMPL_HELPER_CAST(Loff_t)
NONDET_IMPL_HELPER_CAST(Sector_t)
NONDET_IMPL_HELPER_POINTER(Pchar)
NONDET_IMPL_HELPER_POINTER(Pointer)

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
