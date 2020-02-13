/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#ifndef MODULES_BACKEND_PASS_DEBUGINFO_HPP_
#define MODULES_BACKEND_PASS_DEBUGINFO_HPP_

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Metadata.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// using namespace llvm;
using llvm::ConstantInt;
using llvm::DebugLoc;
using llvm::Instruction;
using llvm::LLVMContext;
using llvm::StringRef;
using llvm::Type;
using llvm::Value;

struct DebugInfo {
  DebugInfo(LLVMContext* Ctx, Instruction* i);
  Value* getScopeNumberValue();
  Value* getLineNumberValue();
  unsigned getLineNumberInt();
  StringRef getVarName();

 private:
  Value* scopeNumberValue = NULL;
  Value* lineNumberValue = NULL;
  unsigned lineNumInt = 0;
  StringRef varName;
};

#endif  // MODULES_BACKEND_PASS_DEBUGINFO_HPP_
