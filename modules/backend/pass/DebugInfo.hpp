#pragma once

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Metadata.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace llvm;

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
