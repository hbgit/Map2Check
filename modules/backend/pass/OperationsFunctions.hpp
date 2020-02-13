/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#ifndef MODULES_BACKEND_PASS_OPERATIONSFUNCTIONS_HPP_
#define MODULES_BACKEND_PASS_OPERATIONSFUNCTIONS_HPP_

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

class OperationsFunctions {
  Constant *OverflowAdd = NULL;
  Constant *OverflowAddUint = NULL;
  Constant *OverflowSub = NULL;
  Constant *OverflowSubUint = NULL;
  Constant *OverflowMul = NULL;
  Constant *OverflowMulUint = NULL;
  Constant *OverflowSDiv = NULL;
  Constant *OverflowError = NULL;

 public:
  Constant *getOverflowAdd() { return this->OverflowAdd; }
  Constant *getOverflowAddUint() { return this->OverflowAddUint; }
  Constant *getOverflowSub() { return this->OverflowSub; }
  Constant *getOverflowSubUint() { return this->OverflowSubUint; }
  Constant *getOverflowMul() { return this->OverflowMul; }
  Constant *getOverflowMulUint() { return this->OverflowMulUint; }
  Constant *getOverflowSDiv() { return this->OverflowSDiv; }
  Constant *getOverflowError() { return this->OverflowError; }

  OperationsFunctions(Function *F, LLVMContext *Ctx) {
    this->OverflowAdd = F->getParent()->getOrInsertFunction(
        "map2check_binop_add", Type::getVoidTy(*Ctx), Type::getInt64Ty(*Ctx),
        Type::getInt64Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt8PtrTy(*Ctx));

    this->OverflowAddUint = F->getParent()->getOrInsertFunction(
        "map2check_binop_add_uint", Type::getVoidTy(*Ctx),
        Type::getInt64Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt64Ty(*Ctx), Type::getInt8PtrTy(*Ctx));

    this->OverflowSub = F->getParent()->getOrInsertFunction(
        "map2check_binop_sub", Type::getVoidTy(*Ctx), Type::getInt64Ty(*Ctx),
        Type::getInt64Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt8PtrTy(*Ctx));

    this->OverflowSubUint = F->getParent()->getOrInsertFunction(
        "map2check_binop_sub_uint", Type::getVoidTy(*Ctx),
        Type::getInt64Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt64Ty(*Ctx), Type::getInt8PtrTy(*Ctx));

    this->OverflowMul = F->getParent()->getOrInsertFunction(
        "map2check_binop_mul", Type::getVoidTy(*Ctx), Type::getInt64Ty(*Ctx),
        Type::getInt64Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt8PtrTy(*Ctx));

    this->OverflowMulUint = F->getParent()->getOrInsertFunction(
        "map2check_binop_mul_uint", Type::getVoidTy(*Ctx),
        Type::getInt64Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt64Ty(*Ctx), Type::getInt8PtrTy(*Ctx));

    this->OverflowSDiv = F->getParent()->getOrInsertFunction(
        "map2check_binop_sdiv", Type::getVoidTy(*Ctx), Type::getInt64Ty(*Ctx),
        Type::getInt64Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt8PtrTy(*Ctx));

    this->OverflowError = F->getParent()->getOrInsertFunction(
        "overflowError", Type::getVoidTy(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt8PtrTy(*Ctx));
  }
};

#endif  // MODULES_BACKEND_PASS_OPERATIONSFUNCTIONS_HPP_
