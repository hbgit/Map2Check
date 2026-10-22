/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

// Memory tracking against the core API of map2check-library: every runtime
// call is a RuntimeHook (RuntimeABI.hpp), with the argument order and widths
// of docs/cutover/abi-mapping.tsv.

#include "MemoryTrackPass.hpp"

#include <vector>

#include <llvm/IR/IntrinsicInst.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>

#include "RuntimeABI.hpp"

static llvm::cl::opt<std::string> MemTrackEntryFunction(
    "m2c-entry-function",
    llvm::cl::desc("Name of the entry function for MemoryTrackPass"),
    llvm::cl::init("main"));

static llvm::cl::opt<bool> WasmMode(
    "wasm-mode",
    llvm::cl::desc("Enable WASM bounds checking on linear memory accesses"),
    llvm::cl::init(false));

// using namespace llvm;
using llvm::AllocaInst;
using llvm::BasicBlock;
using llvm::CallInst;
using llvm::ConstantInt;
using llvm::DataLayout;
using llvm::DebugLoc;
using llvm::dyn_cast;
using llvm::GetElementPtrInst;
using llvm::GlobalVariable;
using llvm::Instruction;
using llvm::IRBuilder;
using llvm::LoadInst;
using llvm::Module;
using llvm::StoreInst;
using llvm::Type;
using llvm::Value;
using Map2Check::RuntimeHook;

namespace {
inline Instruction *BBIteratorToInst(BasicBlock::iterator i) {
  Instruction *pointer = reinterpret_cast<Instruction *>(&*i);
  return pointer;
}

/// True for what a pass added for the runtime's own use (NonDetPass's value
/// slots): not program memory, so neither tracked nor checked.
bool isRuntimeOwned(const Value *value) {
  auto *inst = dyn_cast<Instruction>(value->stripPointerCasts());
  return inst != nullptr && inst->hasMetadata(Map2Check::kRuntimeMetadata);
}
}  // namespace

/** Widens or narrows an integer operand to the type the runtime call declares.
 *
 * The allocation size is taken straight from the program's own call, and its
 * type is whatever that program declared, while the core API takes int32_t
 * sizes. A call whose argument type did not match its signature made LLVM
 * reject the module outright:
 *
 *   Call parameter type does not match function signature!
 *   LLVM ERROR: Broken module found, compilation aborted!
 *
 * Not a corner case. On the Test-Comp corpus this killed all 110 ProductLines
 * tasks, two seconds into a sixty-second budget, plus a share of ECA -- the
 * CIL-processed sources there allocate with i32. It went unseen because the
 * Juliet and CASTLE programs allocate with i64, so every benchmark this tool
 * is regularly run against happened to match. */
static Value *coerceToParamType(IRBuilder<> &builder, Value *value,
                                FunctionCallee callee, unsigned argIndex) {
  Type *wanted = callee.getFunctionType()->getParamType(argIndex);
  if (value->getType() == wanted) return value;
  if (value->getType()->isIntegerTy() && wanted->isIntegerTy()) {
    // Signed: an allocation size is non-negative in practice, but a program is
    // free to compute one in a signed type, and truncating a sign-extended
    // value back is what preserves it either way.
    return builder.CreateSExtOrTrunc(value, wanted);
  }
  if (value->getType()->isPointerTy() && wanted->isPointerTy()) {
    return builder.CreatePointerCast(value, wanted);
  }
  return value;
}

static ConstantInt *i32Constant(LLVMContext *ctx, uint64_t value) {
  return ConstantInt::get(Type::getInt32Ty(*ctx), value);
}

Value *MemoryTrackPass::functionName() {
  if (this->functionNameValue == nullptr) {
    IRBuilder<> builder(
        &*this->currentFunction->getEntryBlock().getFirstInsertionPt());
    this->functionNameValue =
        builder.CreateGlobalStringPtr(this->currentFunction->getName());
  }
  return this->functionNameValue;
}

// abi-mapping: map2check_add_store_pointer -> map2check_map_store_pointer
// (var, value, scope, name, line, fn), after the store.
void MemoryTrackPass::instrumentPointer() {
  StoreInst *storeInst = dyn_cast<StoreInst>(&*this->currentInstruction);
  Value *var_address = storeInst->getPointerOperand();
  Value *receives = storeInst->getValueOperand();

  IRBuilder<> builder(storeInst->getNextNode());
  Value *name_llvm = builder.CreateGlobalStringPtr(var_address->getName());

  Value *args[] = {coerceToParamType(builder, var_address, storePointer, 0),
                   coerceToParamType(builder, receives, storePointer, 1),
                   this->scope_value,
                   name_llvm,
                   this->line_value,
                   this->functionName()};
  builder.CreateCall(this->storePointer, args);
}

// abi-mapping: map2check_posix -> map2check_map_posix_memalign(memptr, i32).
void MemoryTrackPass::instrumentPosixMemAllign() {
  CallInst *callInst = dyn_cast<CallInst>(&*this->currentInstruction);

  IRBuilder<> builder(callInst->getNextNode());
  Value *size = callInst->getArgOperand(2);
  Value *pointer = callInst->getArgOperand(0);

  Value *args[] = {coerceToParamType(builder, pointer, mapPosixMemalign, 0),
                   coerceToParamType(builder, size, mapPosixMemalign, 1)};
  builder.CreateCall(mapPosixMemalign, args);
}

// abi-mapping: map2check_malloc -> map2check_map_malloc(block, i32 size).
void MemoryTrackPass::instrumentMalloc() {
  CallInst *callInst = dyn_cast<CallInst>(&*this->currentInstruction);
  IRBuilder<> builder(callInst->getNextNode());
  Value *size = callInst->getArgOperand(0);
  Value *args[] = {callInst, coerceToParamType(builder, size, mapMalloc, 1)};
  builder.CreateCall(mapMalloc, args);
}

// realloc(old, n): the old block goes through the resolved-address free check
// (NULL is valid here, is_null_valid 1), which records the release when it
// passes; the new block is mapped as a malloc.
void MemoryTrackPass::instrumentRealloc() {
  CallInst *callInst = dyn_cast<CallInst>(&*this->currentInstruction);
  IRBuilder<> freeBuilder(callInst);

  Value *nullValid = ConstantInt::get(Type::getInt16Ty(*this->Ctx), 1);
  Value *freeArgs[] = {
      coerceToParamType(freeBuilder, callInst->getArgOperand(0),
                        checkFreeResolved, 0),
      this->line_value, this->functionName(), nullValid};
  freeBuilder.CreateCall(checkFreeResolved, freeArgs);

  IRBuilder<> builder(callInst->getNextNode());
  Value *size = callInst->getArgOperand(1);
  Value *args[] = {callInst, coerceToParamType(builder, size, mapMalloc, 1)};
  builder.CreateCall(mapMalloc, args);
}

// abi-mapping: map2check_load + map2check_check_deref -> map2check_check_load
// (ptr, line, scope, size, fn); check_load records the violation itself.
// BUG on map2check_load
void MemoryTrackPass::instrumentMemset() {
  CallInst *callInst = dyn_cast<CallInst>(&*this->currentInstruction);
  IRBuilder<> builder(callInst);

  Value *size = callInst->getArgOperand(2);
  Value *pointer = callInst->getArgOperand(0);

  Value *args[] = {coerceToParamType(builder, pointer, checkLoad, 0),
                   this->line_value, this->scope_value,
                   coerceToParamType(builder, size, checkLoad, 3),
                   this->functionName()};
  builder.CreateCall(checkLoad, args);
}

void MemoryTrackPass::instrumentMemcpy() {
  CallInst *callInst = dyn_cast<CallInst>(&*this->currentInstruction);
  IRBuilder<> builder(callInst);

  Value *size = coerceToParamType(builder, callInst->getArgOperand(2),
                                  checkLoad, 3);
  for (Value *pointer :
       {callInst->getArgOperand(1), callInst->getArgOperand(0)}) {
    Value *args[] = {coerceToParamType(builder, pointer, checkLoad, 0),
                     this->line_value, this->scope_value, size,
                     this->functionName()};
    builder.CreateCall(checkLoad, args);
  }
}

// alloca(n) called as a function: a static block of n bytes.
void MemoryTrackPass::instrumentAlloca() {
  CallInst *callInst = dyn_cast<CallInst>(&*this->currentInstruction);
  IRBuilder<> builder(callInst->getNextNode());
  Value *size = coerceToParamType(builder, callInst->getArgOperand(0),
                                  mapAlloca, 2);

  Value *function_llvm = builder.CreateGlobalStringPtr("");
  Value *args[] = {function_llvm, callInst,         size,
                   size,          this->line_value, this->scope_value};
  builder.CreateCall(mapAlloca, args);
}

/* For the purpose of memory checking calloc is basically
 * a malloc with 2 args, the first is how many elements
 * and the second is the size of the primitive element
 */
void MemoryTrackPass::instrumentCalloc() {
  CallInst *callInst = dyn_cast<CallInst>(&*this->currentInstruction);
  IRBuilder<> builder(callInst->getNextNode());
  Value *args[] = {
      callInst,
      coerceToParamType(builder, callInst->getArgOperand(0), mapCalloc, 1),
      coerceToParamType(builder, callInst->getArgOperand(1), mapCalloc, 2)};
  builder.CreateCall(mapCalloc, args);
}

// abi-mapping: map2check_free -> map2check_check_free + map2check_map_free.
// free(var) is recorded here and instrumented after the walk; free of any
// other address is one resolved-address check (is_null_valid 0).
void MemoryTrackPass::instrumentFree() {
  CallInst *callInst = dyn_cast<CallInst>(&*this->currentInstruction);
  auto *li =
      dyn_cast<LoadInst>(callInst->getArgOperand(0)->stripPointerCasts());

  if (li != nullptr && li->getType()->isPointerTy()) {
    this->pendingFrees.push_back(
        {callInst, li->getPointerOperand(), li, this->line_value,
         this->scope_value});
    return;
  }

  IRBuilder<> builder(callInst);
  Value *nullNotValid = ConstantInt::get(Type::getInt16Ty(*this->Ctx), 0);
  Value *args[] = {coerceToParamType(builder, callInst->getArgOperand(0),
                                     checkFreeResolved, 0),
                   this->line_value, this->functionName(), nullNotValid};
  builder.CreateCall(checkFreeResolved, args);
}

// The core keys the release on the heap address, and the C runtime read it
// from the pointer variable's slot: the block is the program's own load of
// *slot (the value free receives; no second load), both calls are skipped
// for NULL as the C runtime did, else check_free then map_free, both
// (name, slot, *slot, scope, line, fn).
void MemoryTrackPass::instrumentPendingFrees() {
  for (const PendingFree &pending : this->pendingFrees) {
    IRBuilder<> builder(pending.call);
    Value *name_llvm =
        builder.CreateGlobalStringPtr(pending.slot->getName());
    Value *slot = coerceToParamType(builder, pending.slot, checkFree, 1);
    LoadInst *block = pending.block;
    Value *nonNull = builder.CreateIsNotNull(block);
    Instruction *then = llvm::SplitBlockAndInsertIfThen(
        nonNull, pending.call, /*Unreachable=*/false);
    IRBuilder<> guarded(then);
    Value *args[] = {name_llvm,     slot,         block,
                     pending.scope, pending.line, this->functionName()};
    guarded.CreateCall(checkFree, args)->setDebugLoc(pending.call->getDebugLoc());
    guarded.CreateCall(mapFree, args)->setDebugLoc(pending.call->getDebugLoc());
  }
  this->pendingFrees.clear();
}

void MemoryTrackPass::getDebugInfo() {
  unsigned scope_number;
  unsigned line_number;
  DebugLoc location = this->currentInstruction->getDebugLoc();
  if (location) {
    scope_number = location.getScope()->getMetadataID();
    line_number = location.getLine();

  } else {
    scope_number = 0;
    line_number = 0;
  }

  this->scope_value = i32Constant(this->Ctx, scope_number);
  this->line_value = i32Constant(this->Ctx, line_number);
}

int MemoryTrackPass::getLineNumber() {
  unsigned line_number;
  DebugLoc location = this->currentInstruction->getDebugLoc();
  if (location) {
    line_number = location.getLine();
  } else {
    line_number = 0;
  }
  return line_number;
}

// Globals are mapped as static blocks at the start of the entry function.
void MemoryTrackPass::instrumentInit() {
  Function::iterator bb = this->currentFunction->begin();
  BasicBlock::iterator i = bb->begin();

  IRBuilder<> builder(BBIteratorToInst(i));
  Module *currentModule = this->currentFunction->getParent();

  std::vector<GlobalVariable *> globals;
  for (auto globalVar = currentModule->global_begin();
       globalVar != currentModule->global_end(); globalVar++) {
    GlobalVariable *variable = dyn_cast<GlobalVariable>(&*globalVar);
    globals.push_back(variable);
  }

  for (unsigned pos = 0; pos < globals.size(); pos++) {
    GlobalVariable *variable = globals[pos];
    const DataLayout dataLayout = currentModule->getDataLayout();
    auto type = variable->getValueType();
    unsigned typeSize = dataLayout.getTypeSizeInBits(type) / 8;
    unsigned primitiveSize = 0;

    if (type->isArrayTy()) {
      type = type->getArrayElementType();
    }

    if (type->isVectorTy()) {
      type = type->getScalarType();
    }

    primitiveSize = dataLayout.getTypeSizeInBits(type) / 8;

    Value *name_llvm = builder.CreateGlobalStringPtr(variable->getName());

    // TODO(hbgit):
    this->currentInstruction = i;
    this->getDebugInfo();

    // (name, addr, size, size_of_primitive, line, scope)
    Value *args[] = {name_llvm,
                     variable,
                     i32Constant(this->Ctx, typeSize),
                     i32Constant(this->Ctx, primitiveSize),
                     this->line_value,
                     this->scope_value};
    builder.CreateCall(mapAlloca, args);
  }
}

// TODO(hbgit): use hash table instead of nested "if's"
bool MemoryTrackPass::isWasmAllocator(llvm::StringRef name) {
  if (!name.startswith("w2c_")) return false;
  if (name.contains("wasm_rt_")) return false;
  return name.endswith("_dlmalloc") || name.endswith("_malloc");
}

bool MemoryTrackPass::isWasmDeallocator(llvm::StringRef name) {
  if (!name.startswith("w2c_")) return false;
  if (name.contains("wasm_rt_")) return false;
  return name.endswith("_dlfree") || name.endswith("_free");
}

// abi-mapping: map2check_wasm_malloc(u64 offset, u64 size).
void MemoryTrackPass::instrumentWasmMalloc() {
  CallInst *callInst = dyn_cast<CallInst>(&*this->currentInstruction);
  IRBuilder<> builder(callInst->getNextNode());

  // Arg 1 = size (i32)
  Value *size32 = callInst->getArgOperand(1);
  Value *size64 = builder.CreateZExt(size32, Type::getInt64Ty(*this->Ctx));

  // callInst (return value) = offset i32 → i64
  Value *offset64 = builder.CreateZExt(callInst, Type::getInt64Ty(*this->Ctx));

  Value *args[] = {offset64, size64};
  builder.CreateCall(wasmMalloc, args);
}

// abi-mapping: map2check_wasm_free(u64 offset, i64 line).
void MemoryTrackPass::instrumentWasmFree() {
  CallInst *callInst = dyn_cast<CallInst>(&*this->currentInstruction);
  IRBuilder<> builder(callInst);

  // Arg 1 = offset (i32) → i64
  Value *offset32 = callInst->getArgOperand(1);
  Value *offset64 = builder.CreateZExt(offset32, Type::getInt64Ty(*this->Ctx));
  Value *line64 =
      builder.CreateSExt(this->line_value, Type::getInt64Ty(*this->Ctx));

  Value *args[] = {offset64, line64};
  builder.CreateCall(wasmFree, args);
}

void MemoryTrackPass::switchCallInstruction() {
  llvm::StringRef calleeName = this->calleeFunction->getName();

  // WASM mode: intercept wasm2c allocator functions
  if (WasmModeActive) {
    if (isWasmAllocator(calleeName)) {
      this->instrumentWasmMalloc();
      return;
    }
    if (isWasmDeallocator(calleeName)) {
      this->instrumentWasmFree();
      return;
    }
  }

  // TODO(hbgit): Resolve SVCOMP ISSUE
  if (this->calleeFunction->getName() == "free") {
    this->instrumentFree();
  } else if (this->calleeFunction->getName() == "cfree") {
    this->instrumentFree();
  } else if (this->calleeFunction->getName() == "posix_memalign") {
    this->instrumentPosixMemAllign();
  } else if (llvm::isa<llvm::MemSetInst>(&*this->currentInstruction) ||
             calleeName == "memset") {
    // By the intrinsic's kind, not its name (Map2Check 9.0, 956df7b7e). The
    // names carried the pointer types -- llvm.memset.p0i8.i64 -- and with
    // opaque pointers LLVM 16 names them llvm.memset.p0.i64: matched by name,
    // no memset, memcpy or memmove clang emitted was checked at all
    // (memset(p, 0, 16) on 8 malloc'd bytes came back UNKNOWN).
    this->instrumentMemset();
  } else if (llvm::isa<llvm::MemTransferInst>(&*this->currentInstruction) ||
             calleeName == "memcpy" || calleeName == "memmove") {
    this->instrumentMemcpy();
  } else if (this->calleeFunction->getName() == "malloc") {
    this->instrumentMalloc();
  } else if (this->calleeFunction->getName() == "valloc") {
    this->instrumentMalloc();
  } else if (this->calleeFunction->getName() == "alloca") {
    this->instrumentAlloca();
  } else if (this->calleeFunction->getName() == "calloc") {
    this->instrumentCalloc();
  } else if (this->calleeFunction->getName() == "realloc") {
    this->instrumentRealloc();
  }
}

// abi-mapping: map2check_function -> map2check_map_funct_address(name, addr).
// Not called from run() (as before the cutover).
void MemoryTrackPass::instrumentFunctionAddress() {
  if (!this->mainFunctionInitialized) {
    this->functionsValues.push_back(this->currentFunction);
    return;
  }

  Function::iterator bb = this->mainFunction->begin();
  BasicBlock::iterator i = bb->begin();
  i++;

  IRBuilder<> builder(BBIteratorToInst(i));
  for (unsigned iterator = 0; iterator < this->functionsValues.size();
       iterator++) {
    Value *name_llvm = builder.CreateGlobalStringPtr(
        this->functionsValues[iterator]->getName());
    Value *instrumentationArgs[] = {name_llvm,
                                    this->functionsValues[iterator]};
    builder.CreateCall(mapFunctAddress, instrumentationArgs);
  }

  this->functionsValues.clear();
}

void MemoryTrackPass::instrumentAllocation() {
  AllocaInst *allocaInst = dyn_cast<AllocaInst>(&*this->currentInstruction);

  Module *M = this->currentFunction->getParent();
  const DataLayout dataLayout = M->getDataLayout();

  auto type = allocaInst->getAllocatedType();

  unsigned typeSize = dataLayout.getTypeSizeInBits(type) / 8;

  unsigned primitiveSize = 0;

  if (type->isArrayTy()) {
    type = type->getArrayElementType();
  }

  if (type->isVectorTy()) {
    type = type->getScalarType();
  }

  primitiveSize = dataLayout.getTypeSizeInBits(type) / 8;
  IRBuilder<> builder(allocaInst->getNextNode());
  Value *name_llvm = builder.CreateGlobalStringPtr(allocaInst->getName());

  Value *args[] = {name_llvm,
                   allocaInst,
                   i32Constant(this->Ctx, typeSize),
                   i32Constant(this->Ctx, primitiveSize),
                   this->line_value,
                   this->scope_value};
  builder.CreateCall(mapAlloca, args);
}

// TODO(hbgit): make dynCast only one time
void MemoryTrackPass::runOnCallInstruction() {
  CallInst *callInst = dyn_cast<CallInst>(&*this->currentInstruction);

  this->calleeFunction = callInst->getCalledFunction();

  if (this->calleeFunction == NULL) {
    Value *v = callInst->getCalledOperand();
    this->calleeFunction = dyn_cast<Function>(v->stripPointerCasts());

    if (this->calleeFunction != NULL) {
      this->switchCallInstruction();
    }
  } else {
    this->switchCallInstruction();
  }

  this->calleeFunction = NULL;
}

// A store is checked like a load of the stored size, before it happens.
void MemoryTrackPass::runOnStoreInstruction() {
  StoreInst *storeInst = dyn_cast<StoreInst>(&*this->currentInstruction);
  if (storeInst->getValueOperand()->getType()->isPointerTy()) {
    this->instrumentPointer();
  }

  Value *var_address = storeInst->getPointerOperand();
  Value *receives = storeInst->getValueOperand();

  Module *M = this->currentFunction->getParent();
  const DataLayout dataLayout = M->getDataLayout();

  unsigned typeSize = dataLayout.getTypeSizeInBits(receives->getType()) / 8;

  IRBuilder<> builder(storeInst);
  Value *args[] = {coerceToParamType(builder, var_address, checkLoad, 0),
                   this->line_value, this->scope_value,
                   i32Constant(this->Ctx, typeSize), this->functionName()};
  builder.CreateCall(checkLoad, args);
}

// abi-mapping: map2check_non_static_alloca: the size is the element count;
// the core records count * size_primitive bytes.
void MemoryTrackPass::instrumentNotStaticArrayAlloca() {
  AllocaInst *allocaInst = dyn_cast<AllocaInst>(&*this->currentInstruction);
  Value *v = allocaInst->getArraySize();

  Module *M = this->currentFunction->getParent();
  const DataLayout dataLayout = M->getDataLayout();

  auto type = allocaInst->getAllocatedType();
  unsigned primitiveSize = dataLayout.getTypeSizeInBits(type) / 8;
  IRBuilder<> builder(allocaInst->getNextNode());
  Value *name_llvm = builder.CreateGlobalStringPtr(allocaInst->getName());

  Value *args[] = {name_llvm,
                   allocaInst,
                   coerceToParamType(builder, v, mapNonStaticAlloca, 2),
                   i32Constant(this->Ctx, primitiveSize),
                   this->line_value,
                   this->scope_value};
  builder.CreateCall(mapNonStaticAlloca, args);
}

void MemoryTrackPass::instrumentArrayAlloca() {
  AllocaInst *allocaInst = dyn_cast<AllocaInst>(&*this->currentInstruction);
  Value *v = allocaInst->getArraySize();

  Module *M = this->currentFunction->getParent();
  const DataLayout dataLayout = M->getDataLayout();

  auto type = v->getType();
  unsigned primitiveSize = dataLayout.getTypeSizeInBits(type) / 8;
  IRBuilder<> builder(allocaInst->getNextNode());
  Value *name_llvm = builder.CreateGlobalStringPtr(allocaInst->getName());

  Value *args[] = {name_llvm,
                   allocaInst,
                   coerceToParamType(builder, v, mapAlloca, 2),
                   i32Constant(this->Ctx, primitiveSize),
                   this->line_value,
                   this->scope_value};
  builder.CreateCall(mapAlloca, args);
}

void MemoryTrackPass::runOnAllocaInstruction() {
  AllocaInst *allocaInst = dyn_cast<AllocaInst>(&*this->currentInstruction);

  if (allocaInst->isArrayAllocation()) {
    if (allocaInst->isStaticAlloca()) {
      this->instrumentArrayAlloca();
    } else {
      this->instrumentNotStaticArrayAlloca();
    }

  } else {
    this->instrumentAllocation();
  }
}

// abi-mapping: map2check_load (+ check_deref) -> map2check_check_load
// (ptr, line, scope, size, fn), before the load.
void MemoryTrackPass::runOnLoadInstruction() {
  LoadInst *loadInst = dyn_cast<LoadInst>(&*this->currentInstruction);

  Value *v = loadInst->getPointerOperand()->stripPointerCasts();

  Module *M = this->currentFunction->getParent();
  const DataLayout dataLayout = M->getDataLayout();

  unsigned typeSize = dataLayout.getTypeSizeInBits(loadInst->getType()) / 8;

  IRBuilder<> builder(loadInst);
  Value *args[] = {coerceToParamType(builder, v, checkLoad, 0),
                   this->line_value, this->scope_value,
                   i32Constant(this->Ctx, typeSize), this->functionName()};
  builder.CreateCall(checkLoad, args);
}

void MemoryTrackPass::prepareMap2CheckInstructions() {
  Module &module = *this->currentFunction->getParent();

  this->checkLoad = Map2Check::getOrInsertRuntime(module, RuntimeHook::Load);
  this->checkFreeResolved =
      Map2Check::getOrInsertRuntime(module, RuntimeHook::FreeResolved);
  this->storePointer =
      Map2Check::getOrInsertRuntime(module, RuntimeHook::StorePointer);
  this->mapMalloc = Map2Check::getOrInsertRuntime(module, RuntimeHook::Malloc);
  this->mapPosixMemalign =
      Map2Check::getOrInsertRuntime(module, RuntimeHook::PosixMemalign);
  this->mapCalloc = Map2Check::getOrInsertRuntime(module, RuntimeHook::Calloc);
  this->mapAlloca = Map2Check::getOrInsertRuntime(module, RuntimeHook::Alloca);
  this->mapNonStaticAlloca =
      Map2Check::getOrInsertRuntime(module, RuntimeHook::NonStaticAlloca);
  this->mapFunctAddress =
      Map2Check::getOrInsertRuntime(module, RuntimeHook::FunctionAddr);
  this->checkFree =
      Map2Check::getOrInsertRuntime(module, RuntimeHook::CheckFree);
  this->mapFree = Map2Check::getOrInsertRuntime(module, RuntimeHook::MapFree);

  if (WasmModeActive) {
    this->wasmMalloc =
        Map2Check::getOrInsertRuntime(module, RuntimeHook::WasmMalloc);
    this->wasmFree = Map2Check::getOrInsertRuntime(module, RuntimeHook::WasmFree);
    this->wasmCheckAccess =
        Map2Check::getOrInsertRuntime(module, RuntimeHook::WasmCheckAccess);
  }
}

PreservedAnalyses MemoryTrackPass::run(Function &F,
                                       llvm::FunctionAnalysisManager &AM) {
  this->Ctx = &F.getContext();
  this->currentFunction = &F;
  this->functionNameValue = nullptr;
  this->pendingFrees.clear();
  if (F.isDeclaration()) return PreservedAnalyses::all();
  this->prepareMap2CheckInstructions();
  // this->instrumentInit(); //overhead BUG

  if (F.getName() == MemTrackEntryFunction) {
    this->functionsValues.push_back(this->currentFunction);
    this->mainFunctionInitialized = true;
    this->mainFunction = &F;
    this->instrumentInit();
  }

  for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
    for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {
      this->currentInstruction = i;
      if (i->hasMetadata(Map2Check::kRuntimeMetadata)) continue;

      if (dyn_cast<CallInst>(&*i) != NULL) {
        this->getDebugInfo();
        this->runOnCallInstruction();
      } else if (auto *SI = dyn_cast<StoreInst>(&*this->currentInstruction)) {
        if (isRuntimeOwned(SI->getPointerOperand())) continue;
        this->getDebugInfo();
        this->runOnStoreInstruction();
        if (WasmModeActive) {
          instrumentWasmBoundsCheck(&*i, SI->getPointerOperand(),
                                    SI->getValueOperand()->getType());
        }
      } else if (dyn_cast<AllocaInst>(&*this->currentInstruction) != NULL) {
        this->getDebugInfo();
        this->runOnAllocaInstruction();
      } else if (auto *LI = dyn_cast<LoadInst>(&*this->currentInstruction)) {
        if (isRuntimeOwned(LI->getPointerOperand())) continue;
        this->getDebugInfo();
        this->runOnLoadInstruction();
        if (WasmModeActive) {
          instrumentWasmBoundsCheck(&*i, LI->getPointerOperand(),
                                    LI->getType());
        }
      }
    }
  }

  // After the walk: guarding a free splits its block.
  this->instrumentPendingFrees();

  return PreservedAnalyses::none();
}

// abi-mapping: map2check_wasm_check_access(u64 offset, u64 access_size).
void MemoryTrackPass::instrumentWasmBoundsCheck(llvm::Instruction* I,
                                                  llvm::Value* ptr,
                                                  llvm::Type* accessType) {
  // Extract offset from GEP into w2c_memory.data
  Value* stripped = ptr->stripPointerCasts();
  auto* GEP = dyn_cast<GetElementPtrInst>(stripped);
  if (!GEP) return;

  if (GEP->getNumOperands() < 3) return;
  Value* offset = GEP->getOperand(GEP->getNumOperands() - 1);
  if (!offset) return;

  Module* M = I->getModule();
  if (!M) return;
  const DataLayout DL(M);
  uint64_t accessSize = DL.getTypeStoreSize(accessType);
  auto* int64Ty = Type::getInt64Ty(I->getContext());

  IRBuilder<> builder(I);
  Value* offset64 = builder.CreateZExtOrTrunc(offset, int64Ty);
  Value* sizeVal = ConstantInt::get(int64Ty, accessSize);

  Value* args[] = {offset64, sizeVal};
  builder.CreateCall(wasmCheckAccess, args);
}

// --- New Pass Manager plugin registration ---
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MemoryTrackPass", LLVM_VERSION_STRING,
          [](llvm::PassBuilder& PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager& FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "memory-track") {
                    FPM.addPass(MemoryTrackPass(false, WasmMode.getValue()));
                    return true;
                  }
                  return false;
                });
          }};
}
