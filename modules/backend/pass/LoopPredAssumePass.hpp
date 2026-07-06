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
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Scalar/LoopPassManager.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <memory>

#include "../../frontend/utils/tools.hpp"
#include "DebugInfo.hpp"
#include "LibraryFunctions.hpp"

namespace Tools = Map2Check;

using llvm::BasicBlock;
using llvm::BranchInst;
using llvm::CmpInst;
using llvm::Constant;
using llvm::FunctionCallee;
using llvm::PointerType;
using llvm::dyn_cast;
using llvm::LLVMContext;
using llvm::Loop;
using llvm::PreservedAnalyses;

struct LoopPredAssumePass : public llvm::PassInfoMixin<LoopPredAssumePass> {
  PreservedAnalyses run(Loop& L, llvm::LoopAnalysisManager& AM,
                        llvm::LoopStandardAnalysisResults& AR,
                        llvm::LPMUpdater& U);
  static bool isRequired() { return true; }

  void getConditionInLoop(Loop* L);

 private:
  LLVMContext* Ctx;
  FunctionCallee map2check_assume;
};

#endif  // MODULES_BACKEND_PASS_LOOPPREDASSUMEPASS_HPP_
