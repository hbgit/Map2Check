#include "Map2CheckLibrary.hpp"

bool Map2CheckLibrary::runOnFunction(Function& F) {
  this->libraryFunctions = make_unique<LibraryFunctions>(&F, &F.getContext());
  Function::iterator functionIterator = F.begin();
  BasicBlock::iterator instructionIterator = functionIterator->begin();

  IRBuilder<> builder((Instruction*)&*instructionIterator);
  this->functionName = builder.CreateGlobalStringPtr(F.getName());
  if (F.getName() == "main") {
    currentInstruction = instructionIterator;
    instrumentStartInstruction(&F.getContext());
  }

  for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
    for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {
      currentInstruction = i;
      if (CallInst* callInst = dyn_cast<CallInst>(&*i)) {
        this->runOnCallInstruction(callInst, &F.getContext());
      }
    }
  }

  if (F.getName() == "main") {
    // currentInstruction--;
    instrumentReleaseInstruction(&F.getContext());
  }
  return true;
}

void Map2CheckLibrary::instrumentStartInstruction(LLVMContext* Ctx) {
  IRBuilder<> builder((Instruction*)&*currentInstruction);
  Value* argument =
      ConstantInt::getSigned(Type::getInt32Ty(*Ctx), this->SVCOMP ? 1 : 0);
  Value* args[] = {argument};
  builder.CreateCall(this->libraryFunctions->getInitFunction(), args);
}

void Map2CheckLibrary::instrumentReleaseInstruction(LLVMContext* Ctx) {
  IRBuilder<> builder((Instruction*)&*currentInstruction);
  builder.CreateCall(this->libraryFunctions->getExitFunction());
}

void Map2CheckLibrary::runOnCallInstruction(CallInst* callInst,
                                            LLVMContext* Ctx) {
  Function* caleeFunction = callInst->getCalledFunction();

  if (caleeFunction == NULL) {
    Value* v = callInst->getCalledValue();
    caleeFunction = dyn_cast<Function>(v->stripPointerCasts());

    if (caleeFunction == NULL) {
      return;
    }
  }

  if (caleeFunction->getName() == "exit") {
    this->instrumentReleaseInstruction(Ctx);
  } else if (caleeFunction->getName() == "abort") {
    this->instrumentReleaseInstruction(Ctx);
  }
}

char Map2CheckLibrary::ID = 2;
static RegisterPass<Map2CheckLibrary> X(
    "map2check",
    "Adds support for map2check library (MUST be executed before other pass)");
