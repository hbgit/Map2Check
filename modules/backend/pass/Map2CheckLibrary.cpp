/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#include "Map2CheckLibrary.hpp"

#include <memory>

using llvm::CallInst;
using llvm::dyn_cast;
using llvm::IRBuilder;
using llvm::RegisterPass;
using llvm::ReturnInst;
using llvm::Twine;

bool Map2CheckLibrary::runOnFunction(Function& F) {
  this->libraryFunctions = make_unique<LibraryFunctions>(&F, &F.getContext());
  Function::iterator functionIterator = F.begin();
  BasicBlock::iterator instructionIterator = functionIterator->begin();

  IRBuilder<> builder(reinterpret_cast<Instruction*>(&*instructionIterator));
  this->functionName = builder.CreateGlobalStringPtr(F.getName());
  bool isMain = false;
  if (F.getName() == "main") {
    currentInstruction = instructionIterator;
    instrumentStartInstruction(&F.getContext());
    isMain = true;
  }

  for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
    for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {
      currentInstruction = i;
      if (CallInst* callInst = dyn_cast<CallInst>(&*i)) {
        this->runOnCallInstruction(callInst, &F.getContext());
      }
      if (ReturnInst* inst = dyn_cast<ReturnInst>(&*i)) {
        if (isMain) this->instrumentReleaseInstruction(&F.getContext());
      }
    }
  }

  if (F.getName() == "main") {
    // currentInstruction--;
    // instrumentReleaseInstruction(&F.getContext());
    Twine new_entry_name("__map2check_main__");
    F.setName(new_entry_name);
  }

  return true;
}

void Map2CheckLibrary::instrumentStartInstruction(LLVMContext* Ctx) {
  IRBuilder<> builder(reinterpret_cast<Instruction*>(&*currentInstruction));
  Value* argument =
      ConstantInt::getSigned(Type::getInt32Ty(*Ctx), this->SVCOMP ? 1 : 0);
  Value* args[] = {argument};
  builder.CreateCall(this->libraryFunctions->getInitFunction(), args);
}

void Map2CheckLibrary::instrumentReleaseInstruction(LLVMContext* Ctx) {
  IRBuilder<> builder(reinterpret_cast<Instruction*>(&*currentInstruction));
  builder.CreateCall(this->libraryFunctions->getExitFunction());
}

void Map2CheckLibrary::runOnCallInstruction(CallInst* callInst,
                                            LLVMContext* Ctx) {
  Function* calleeFunction = callInst->getCalledFunction();

  if (calleeFunction == NULL) {
    Value* v = callInst->getCalledValue();
    calleeFunction = dyn_cast<Function>(v->stripPointerCasts());

    if (calleeFunction == NULL) {
      return;
    }
  }

  if (calleeFunction->getName() == "exit") {
    this->instrumentReleaseInstruction(Ctx);
  } else if (calleeFunction->getName() == "abort") {
    this->instrumentReleaseInstruction(Ctx);
  }
}

char Map2CheckLibrary::ID = 2;
static RegisterPass<Map2CheckLibrary> X(
    "map2check",
    "Adds support for map2check library (MUST be executed before other pass)");
