/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#ifndef MODULES_BACKEND_PASS_LOOPPREDASSUMEPASS_HPP_
#define MODULES_BACKEND_PASS_LOOPPREDASSUMEPASS_HPP_

#include <llvm/ADT/Statistic.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <memory>

#include "../../frontend/utils/tools.hpp"
#include "DebugInfo.hpp"
#include "LibraryFunctions.hpp"

// using namespace llvm;
namespace Tools = Map2Check;

using llvm::BasicBlock;
using llvm::BranchInst;
using llvm::CmpInst;
using llvm::Constant;
using llvm::dyn_cast;
using llvm::LLVMContext;
using llvm::Loop;
using llvm::LoopPass;
using llvm::LPPassManager;

struct LoopPredAssumePass : public LoopPass {
  static char ID;
  LoopPredAssumePass() : LoopPass(ID) {}

  virtual bool runOnLoop(Loop* L, LPPassManager& LPM);
  void getConditionInLoop(Loop* L);

 private:
  LLVMContext* Ctx;
  Constant* map2check_assume = NULL;
};

#endif  // MODULES_BACKEND_PASS_LOOPPREDASSUMEPASS_HPP_
