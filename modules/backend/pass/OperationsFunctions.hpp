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
using llvm::Function;
using llvm::FunctionCallee;
using llvm::LLVMContext;
using llvm::PointerType;
using llvm::Type;

class OperationsFunctions {
  FunctionCallee OverflowAdd;
  FunctionCallee OverflowAddUint;
  FunctionCallee OverflowSub;
  FunctionCallee OverflowSubUint;
  FunctionCallee OverflowMul;
  FunctionCallee OverflowMulUint;
  FunctionCallee OverflowSDiv;
  FunctionCallee OverflowError;

 public:
  FunctionCallee getOverflowAdd() { return this->OverflowAdd; }
  FunctionCallee getOverflowAddUint() { return this->OverflowAddUint; }
  FunctionCallee getOverflowSub() { return this->OverflowSub; }
  FunctionCallee getOverflowSubUint() { return this->OverflowSubUint; }
  FunctionCallee getOverflowMul() { return this->OverflowMul; }
  FunctionCallee getOverflowMulUint() { return this->OverflowMulUint; }
  FunctionCallee getOverflowSDiv() { return this->OverflowSDiv; }
  FunctionCallee getOverflowError() { return this->OverflowError; }

  OperationsFunctions(Function *F, LLVMContext *Ctx) {
    // LLVM 16: opaque pointers — use PointerType::get(*Ctx, 0) instead of PointerType::get(, 0)
    auto *PtrTy = PointerType::get(*Ctx, 0);

    this->OverflowAdd = F->getParent()->getOrInsertFunction(
        "map2check_binop_add", Type::getVoidTy(*Ctx), Type::getInt64Ty(*Ctx),
        Type::getInt64Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        PtrTy);

    this->OverflowAddUint = F->getParent()->getOrInsertFunction(
        "map2check_binop_add_uint", Type::getVoidTy(*Ctx),
        Type::getInt64Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt64Ty(*Ctx), PtrTy);

    this->OverflowSub = F->getParent()->getOrInsertFunction(
        "map2check_binop_sub", Type::getVoidTy(*Ctx), Type::getInt64Ty(*Ctx),
        Type::getInt64Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        PtrTy);

    this->OverflowSubUint = F->getParent()->getOrInsertFunction(
        "map2check_binop_sub_uint", Type::getVoidTy(*Ctx),
        Type::getInt64Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt64Ty(*Ctx), PtrTy);

    this->OverflowMul = F->getParent()->getOrInsertFunction(
        "map2check_binop_mul", Type::getVoidTy(*Ctx), Type::getInt64Ty(*Ctx),
        Type::getInt64Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        PtrTy);

    this->OverflowMulUint = F->getParent()->getOrInsertFunction(
        "map2check_binop_mul_uint", Type::getVoidTy(*Ctx),
        Type::getInt64Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt64Ty(*Ctx), PtrTy);

    this->OverflowSDiv = F->getParent()->getOrInsertFunction(
        "map2check_binop_sdiv", Type::getVoidTy(*Ctx), Type::getInt64Ty(*Ctx),
        Type::getInt64Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        PtrTy);

    this->OverflowError = F->getParent()->getOrInsertFunction(
        "overflowError", Type::getVoidTy(*Ctx), Type::getInt32Ty(*Ctx),
        PtrTy);
  }
};

#endif  // MODULES_BACKEND_PASS_OPERATIONSFUNCTIONS_HPP_
