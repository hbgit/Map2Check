#pragma once

#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

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

private:
  Value* scopeNumberValue = NULL;
  Value* lineNumberValue = NULL;
};
