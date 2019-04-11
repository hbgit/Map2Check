/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#ifndef MODULES_BACKEND_PASS_NONDETPASS_HPP_
#define MODULES_BACKEND_PASS_NONDETPASS_HPP_

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <memory>

#include "DebugInfo.hpp"
#include "NonDetFunctions.hpp"

// using namespace llvm;
using llvm::BasicBlock;
using llvm::CallInst;
using llvm::Function;
using llvm::FunctionPass;
using llvm::LLVMContext;
using llvm::Value;

enum class NonDetType {
  INTEGER,
  UNSIGNED,
  POINTER,
  //  STRING,
  LONG,
  ASSUME,
  USHORT,
  ULONG,
  CHAR,
  BOOL,
  UCHAR,
  PCHAR,
  SIZE_T,
  LOFF_T,
  SECTOR_T,
  UINT,
  CRAB_ASSUME
};

namespace InstrumentNonDetS {
#define INSTRUMENT_HEADER_HELPER(type) \
  void instrumentNonDet##type(CallInst *callInst, LLVMContext *Ctx);
}  // namespace InstrumentNonDetS

struct NonDetPass : public FunctionPass {
  static char ID;
  NonDetPass() : FunctionPass(ID) {}
  virtual bool runOnFunction(Function &F);

 protected:
  void instrumentInstruction();
  /**
   * @brief Checks if current function is a non det call and instruments it
   */
  void runOnCallInstruction(CallInst *callInst, LLVMContext *Ctx);
  Value *getFunctionNameValue() { return this->functionName; }

 private:
  void instrumentNonDet(NonDetType type, Function *calleeFunction);
  INSTRUMENT_HEADER_HELPER(Integer)
  INSTRUMENT_HEADER_HELPER(Unsigned)
  INSTRUMENT_HEADER_HELPER(Char)
  INSTRUMENT_HEADER_HELPER(Pointer)
  INSTRUMENT_HEADER_HELPER(Long)
  INSTRUMENT_HEADER_HELPER(Ushort)
  INSTRUMENT_HEADER_HELPER(Ulong)
  INSTRUMENT_HEADER_HELPER(Bool)
  INSTRUMENT_HEADER_HELPER(Uchar)
  INSTRUMENT_HEADER_HELPER(Pchar)
  INSTRUMENT_HEADER_HELPER(Size_t)
  INSTRUMENT_HEADER_HELPER(Loff_t)
  INSTRUMENT_HEADER_HELPER(Sector_t)
  INSTRUMENT_HEADER_HELPER(Uint)
  void instrumentCrabAssume(CallInst *callInst, LLVMContext *Ctx);
  std::unique_ptr<NonDetFunctions> nonDetFunctions;
  Value *functionName = NULL;
  BasicBlock::iterator currentInstruction;
};

class NonDetPassException : public std::runtime_error {
 public:
  explicit NonDetPassException(std::string message)
      : std::runtime_error(message) {}
  virtual const char *what() const throw();
};

#endif  // MODULES_BACKEND_PASS_NONDETPASS_HPP_
