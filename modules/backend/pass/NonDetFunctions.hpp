/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#ifndef MODULES_BACKEND_PASS_NONDETFUNCTIONS_HPP_
#define MODULES_BACKEND_PASS_NONDETFUNCTIONS_HPP_

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

// using namespace llvm;
using llvm::Constant;
using llvm::Function;
using llvm::LLVMContext;
using llvm::Type;

#define CONSTANT_GENERATOR(type) \
 private:                        \
  Constant *NonDet##type = NULL; \
                                 \
 public:                         \
  Constant *getNonDet##type##Function() { return this->NonDet##type; }

#define NON_DET_FUNCTIONS_HELPER(type, c_type)                                \
  this->NonDet##type = F->getParent()->getOrInsertFunction(                   \
      "map2check_nondet_" #c_type, Type::getVoidTy(*Ctx),                     \
      Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx), \
      Type::getInt8PtrTy(*Ctx));

#define NON_DET_FUNCTIONS_HELPER_DOUBLE(type, c_type)                                \
  this->NonDet##type = F->getParent()->getOrInsertFunction(                   \
      "map2check_nondet_" #c_type, Type::getVoidTy(*Ctx),                     \
      Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getDoubleTy(*Ctx), \
      Type::getInt8PtrTy(*Ctx));

class NonDetFunctions {
  CONSTANT_GENERATOR(Integer)
  CONSTANT_GENERATOR(Unsigned)
  CONSTANT_GENERATOR(Char)
  CONSTANT_GENERATOR(Pointer)
  CONSTANT_GENERATOR(Long)
  CONSTANT_GENERATOR(Ushort)
  CONSTANT_GENERATOR(Short)
  CONSTANT_GENERATOR(Ulong)
  CONSTANT_GENERATOR(Bool)
  CONSTANT_GENERATOR(Uchar)
  CONSTANT_GENERATOR(Pchar)
  CONSTANT_GENERATOR(Size_t)
  CONSTANT_GENERATOR(Loff_t)
  CONSTANT_GENERATOR(Sector_t)
  CONSTANT_GENERATOR(Uint)
  CONSTANT_GENERATOR(Double)
  // Constant* NonDetAssume = NULL;

 public:
  NonDetFunctions(Function *F, LLVMContext *Ctx) {
    NON_DET_FUNCTIONS_HELPER(Integer, int)
    NON_DET_FUNCTIONS_HELPER(Unsigned, unsigned)
    NON_DET_FUNCTIONS_HELPER(Pointer, pointer)
    NON_DET_FUNCTIONS_HELPER(Ushort, ushort)
    NON_DET_FUNCTIONS_HELPER(Short, short)
    NON_DET_FUNCTIONS_HELPER(Char, char)
    NON_DET_FUNCTIONS_HELPER(Long, long)
    NON_DET_FUNCTIONS_HELPER(Ulong, ulong)
    NON_DET_FUNCTIONS_HELPER(Bool, bool)
    NON_DET_FUNCTIONS_HELPER(Uchar, uchar)
    NON_DET_FUNCTIONS_HELPER(Pchar, pchar)
    NON_DET_FUNCTIONS_HELPER(Size_t, size_t)
    NON_DET_FUNCTIONS_HELPER(Loff_t, loff_t)
    NON_DET_FUNCTIONS_HELPER(Sector_t, sector_t)
    NON_DET_FUNCTIONS_HELPER(Uint, uint)
    NON_DET_FUNCTIONS_HELPER_DOUBLE(Double, double)
  }
};

#endif  // MODULES_BACKEND_PASS_NONDETFUNCTIONS_HPP_
