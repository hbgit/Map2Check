/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#ifndef MODULES_BACKEND_PASS_INITVAR2NONDETFUNCTPASS_HPP_


#define MODULES_BACKEND_PASS_INITVAR2NONDETFUNCTPASS_HPP_


#include <llvm/IR/Function.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Pass.h>
#include <llvm/ADT/None.h>
#include <llvm/Support/Casting.h>

#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <map>
#include "DebugInfo.hpp"

using llvm::BasicBlock;
using llvm::CallInst;
using llvm::Function;
using llvm::FunctionPass;
using llvm::raw_ostream;

struct InitVar2NonDetFunctionPass : public FunctionPass {
    static char ID;
    InitVar2NonDetFunctionPass() : FunctionPass(ID) {}
    virtual bool runOnFunction(Function& F);
};


#endif  // MODULES_BACKEND_PASS_INITVAR2NONDETFUNCTPASS_HPP_
