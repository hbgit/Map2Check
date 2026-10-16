/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#ifndef MODULES_BACKEND_PASS_MEMORYTRACKPASS_HPP_
#define MODULES_BACKEND_PASS_MEMORYTRACKPASS_HPP_

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// using namespace llvm;
using llvm::BasicBlock;
using llvm::Constant;
using llvm::FunctionCallee;
using llvm::PointerType;
using llvm::ConstantInt;
using llvm::Function;
using llvm::LLVMContext;
using llvm::PreservedAnalyses;

struct MemoryTrackPass : public llvm::PassInfoMixin<MemoryTrackPass> {
  explicit MemoryTrackPass(bool SVCOMP = false, bool WasmMode = false)
      : SVCOMP(SVCOMP), WasmModeActive(WasmMode) {}
  PreservedAnalyses run(Function& F, llvm::FunctionAnalysisManager& AM);
  static bool isRequired() { return true; }

 private:
  void instrumentPointer();
  void instrumentMalloc();
  void instrumentCalloc();
  void instrumentRealloc();
  void instrumentMemset();
  void instrumentMemcpy();
  void instrumentPosixMemAllign();
  void instrumentFree();
  void instrumentInit();
  void instrumentAlloca();
  void instrumentArrayAlloca();
  void instrumentNotStaticArrayAlloca();
  void instrumentAllocation();
  void instrumentFunctionAddress();
  void runOnAllocaInstruction();
  void runOnCallInstruction();
  void runOnStoreInstruction();
  void runOnLoadInstruction();
  void switchCallInstruction();
  void prepareMap2CheckInstructions();
  void instrumentWasmBoundsCheck(llvm::Instruction* I, llvm::Value* ptr,
                                   llvm::Type* accessType);
  void instrumentWasmMalloc();
  void instrumentWasmFree();
  bool isWasmAllocator(llvm::StringRef name);
  bool isWasmDeallocator(llvm::StringRef name);
  // void addWitnessInfo(std::string info);
  void getDebugInfo();
  int getLineNumber();

  bool SVCOMP;
  bool WasmModeActive = false;
  bool mainFunctionInitialized = false;
  std::vector<Function*> functionsValues;
  Function* currentFunction;
  // DataLayout* currentDataLayout;
  Function* mainFunction;
  Function* calleeFunction;
  BasicBlock::iterator currentInstruction;
  BasicBlock::iterator lastInstructionMain;
  // Core API of map2check-library (RuntimeABI.hpp), declared per function.
  FunctionCallee storePointer;        // StorePointer
  FunctionCallee mapMalloc;           // Malloc
  FunctionCallee mapCalloc;           // Calloc
  FunctionCallee checkFree;           // CheckFree
  FunctionCallee mapFree;             // MapFree
  FunctionCallee mapAlloca;           // Alloca
  FunctionCallee mapNonStaticAlloca;  // NonStaticAlloca
  FunctionCallee mapPosixMemalign;    // PosixMemalign
  FunctionCallee checkLoad;           // Load
  FunctionCallee mapFunctAddress;     // FunctionAddr
  FunctionCallee checkFreeResolved;   // FreeResolved
  FunctionCallee wasmMalloc;          // WasmMalloc
  FunctionCallee wasmFree;            // WasmFree
  FunctionCallee wasmCheckAccess;     // WasmCheckAccess
  // free(var) calls, guarded after the walk: the NULL check splits blocks.
  struct PendingFree {
    llvm::CallInst* call;
    llvm::Value* slot;
    ConstantInt* line;
    ConstantInt* scope;
  };
  std::vector<PendingFree> pendingFrees;
  llvm::Value* functionNameValue = nullptr;
  llvm::Value* functionName();
  void instrumentPendingFrees();
  ConstantInt* scope_value;
  ConstantInt* line_value;
  LLVMContext* Ctx;
};

class MemoryTrackPassException : public std::runtime_error {
 public:
  explicit MemoryTrackPassException(std::string message)
      : std::runtime_error(message) {}
  virtual const char* what() const throw();
};

#endif  // MODULES_BACKEND_PASS_MEMORYTRACKPASS_HPP_
