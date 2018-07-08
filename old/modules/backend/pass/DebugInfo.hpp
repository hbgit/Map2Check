#pragma once

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Constants.h>

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
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
