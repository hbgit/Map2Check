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
#include <llvm/IR/PassManager.h>
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
  FunctionCallee OverflowSRem;
  FunctionCallee OverflowError;

 public:
  FunctionCallee getOverflowAdd() { return this->OverflowAdd; }
  FunctionCallee getOverflowAddUint() { return this->OverflowAddUint; }
  FunctionCallee getOverflowSub() { return this->OverflowSub; }
  FunctionCallee getOverflowSubUint() { return this->OverflowSubUint; }
  FunctionCallee getOverflowMul() { return this->OverflowMul; }
  FunctionCallee getOverflowMulUint() { return this->OverflowMulUint; }
  FunctionCallee getOverflowSDiv() { return this->OverflowSDiv; }
  FunctionCallee getOverflowSRem() { return this->OverflowSRem; }
  FunctionCallee getOverflowError() { return this->OverflowError; }

  // Signed binop checkers share one ABI:
  //   (i64 lhs, i64 rhs, i32 width, i32 line, i32 scope, ptr functionName)
  //
  // The operands are sign-extended to i64 by the pass (lossless), and `width`
  // carries the bit width of the *result type of the operation*. That is the
  // quantity SV-COMP's no-overflow property is defined over, so the runtime
  // must range-check against it rather than against a fixed width. Keeping a
  // single 64-bit ABI (instead of one symbol per width) mirrors how UBSan's
  // __ubsan_handle_*_overflow handlers take the type as a parameter.
  OperationsFunctions(Function *F, LLVMContext *Ctx) {
    // LLVM 16: opaque pointers — use PointerType::get(*Ctx, 0) instead of PointerType::get(, 0)
    auto *PtrTy = PointerType::get(*Ctx, 0);
    auto *I64 = Type::getInt64Ty(*Ctx);
    auto *I32 = Type::getInt32Ty(*Ctx);
    auto *VoidTy = Type::getVoidTy(*Ctx);

    this->OverflowAdd = F->getParent()->getOrInsertFunction(
        "map2check_binop_add", VoidTy, I64, I64, I32, I32, I32, PtrTy);

    this->OverflowAddUint = F->getParent()->getOrInsertFunction(
        "map2check_binop_add_uint", VoidTy, I64, I64, I32, I32, I32, PtrTy);

    this->OverflowSub = F->getParent()->getOrInsertFunction(
        "map2check_binop_sub", VoidTy, I64, I64, I32, I32, I32, PtrTy);

    this->OverflowSubUint = F->getParent()->getOrInsertFunction(
        "map2check_binop_sub_uint", VoidTy, I64, I64, I32, I32, I32, PtrTy);

    this->OverflowMul = F->getParent()->getOrInsertFunction(
        "map2check_binop_mul", VoidTy, I64, I64, I32, I32, I32, PtrTy);

    this->OverflowMulUint = F->getParent()->getOrInsertFunction(
        "map2check_binop_mul_uint", VoidTy, I64, I64, I32, I32, I32, PtrTy);

    this->OverflowSDiv = F->getParent()->getOrInsertFunction(
        "map2check_binop_sdiv", VoidTy, I64, I64, I32, I32, I32, PtrTy);

    this->OverflowSRem = F->getParent()->getOrInsertFunction(
        "map2check_binop_srem", VoidTy, I64, I64, I32, I32, I32, PtrTy);

    this->OverflowError =
        F->getParent()->getOrInsertFunction("overflowError", VoidTy, I32, PtrTy);
  }
};

#endif  // MODULES_BACKEND_PASS_OPERATIONSFUNCTIONS_HPP_
