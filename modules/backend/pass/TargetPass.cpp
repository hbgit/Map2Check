#include "TargetPass.h"

bool TargetPass::runOnFunction(Function& F) {
  this->targetFunctionMap2Check = F.getParent()->getOrInsertFunction(
      "map2check_target_function", Type::getVoidTy(F.getContext()),
      Type::getInt8PtrTy(F.getContext()), Type::getInt32Ty(F.getContext()),
      Type::getInt32Ty(F.getContext()));

  Function::iterator functionIterator = F.begin();
  BasicBlock::iterator instructionIterator = functionIterator->begin();

  IRBuilder<> builder((Instruction*)&*instructionIterator);
  this->functionName = builder.CreateGlobalStringPtr(F.getName());

  for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
    for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {
      if (CallInst* callInst = dyn_cast<CallInst>(&*i)) {
        currentInstruction = i;
        this->runOnCallInstruction(callInst, &F.getContext());
      }
    }
  }
  return true;
}

void TargetPass::runOnCallInstruction(CallInst* callInst, LLVMContext* Ctx) {
  Function* calleeFunction = callInst->getCalledFunction();

  if (calleeFunction == NULL) {
    Value* v = callInst->getCalledValue();
    calleeFunction = dyn_cast<Function>(v->stripPointerCasts());

    if (calleeFunction == NULL) {
      return;
    }
  }

  if (calleeFunction->getName() == targetFunctionName) {
    this->instrumentErrorInstruction(callInst, Ctx);
  }
}

void TargetPass::instrumentErrorInstruction(CallInst* callInst,
                                            LLVMContext* Ctx) {
  IRBuilder<> builder((Instruction*)&*currentInstruction);
  Value* name_llvm = functionName;

  DebugInfo debugInfo(Ctx, callInst);

  // errs() << *debugInfo.getLineNumberValue() << "----\n";

  Value* args[] = {name_llvm, debugInfo.getScopeNumberValue(),
                   debugInfo.getLineNumberValue()};

  builder.CreateCall(targetFunctionMap2Check, args);
}

char TargetPass::ID = 4;
static RegisterPass<TargetPass> X("target_function",
                                  "Adds map2check calls to check reachability");
