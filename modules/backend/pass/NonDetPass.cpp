#include "NonDetPass.hpp"

namespace {
  Instruction* BBIteratorToInst(BasicBlock::iterator i) {
    Instruction* pointer = reinterpret_cast<Instruction*>(&*i);
    return pointer;
  }
}  // namespace

bool NonDetPass::runOnFunction(Function &F) {
  this->nonDetFunctions =  make_unique<NonDetFunctions>(&F, &F.getContext());
  bool initializedFunctionName = false;
  for (Function::iterator bb = F.begin(), e = F.end();
       bb != e; ++bb) {
    for (BasicBlock::iterator i = bb->begin(),
         e = bb->end(); i != e; ++i) {
      if (!initializedFunctionName) {
        IRBuilder<> builder(BBIteratorToInst(i));
        this->functionName = builder
          .CreateGlobalStringPtr(F.getName());
        initializedFunctionName = true;
      }
      if (CallInst* callInst = dyn_cast<CallInst>(&*i)) {
        currentInstruction = i;
        this->runOnCallInstruction(callInst, &F.getContext());
      }
    }
  }
  return true;
}

void NonDetPass::runOnCallInstruction(CallInst *callInst, LLVMContext* Ctx) {
  Function* caleeFunction = callInst->getCalledFunction();

  if (caleeFunction == NULL) {
    Value* v = callInst->getCalledValue();
    caleeFunction = dyn_cast<Function>(v->stripPointerCasts());
    if (caleeFunction == NULL) {
      return;
    }
  }

  // TODO(rafa.sa.xp@gmail.com): Should not rename __VERIFIER functions
  if ((caleeFunction->getName() == "__VERIFIER_nondet_int")) {
    this->instrumentNonDet(NonDetType::INTEGER, caleeFunction);
    this->instrumentNonDetInt(callInst, Ctx);

  } else if ((caleeFunction->getName() == "map2check_non_det_int")) {
      this->instrumentNonDetInt(callInst, Ctx);

  } else if ((caleeFunction->getName() == "__VERIFIER_nondet_uint")) {
    this->instrumentNonDet(NonDetType::UNSIGNED, caleeFunction);
    this->instrumentNonDetUnsigned(callInst, Ctx);

  } else if ((caleeFunction->getName() == "map2check_non_det_uint")) {
    this->instrumentNonDet(NonDetType::UNSIGNED, caleeFunction);
    this->instrumentNonDetUnsigned(callInst, Ctx);

  } else if ((caleeFunction->getName() == "__VERIFIER_nondet_char")) {
    this->instrumentNonDet(NonDetType::CHAR, caleeFunction);
    this->instrumentNonDetChar(callInst, Ctx);

  } else if ((caleeFunction->getName() == "map2check_non_det_char")) {
    this->instrumentNonDetChar(callInst, Ctx);

  } else if ((caleeFunction->getName() == "__VERIFIER_nondet_pointer")) {
    this->instrumentNonDet(NonDetType::POINTER, caleeFunction);
    this->instrumentNonDetPointer(callInst, Ctx);

  } else if ((caleeFunction->getName() == "map2check_non_det_pointer")) {
    this->instrumentNonDetPointer(callInst, Ctx);

  } else if ((caleeFunction->getName() == "__VERIFIER_nondet_long")) {
    this->instrumentNonDet(NonDetType::LONG, caleeFunction);
    this->instrumentNonDetLong(callInst, Ctx);

  } else if ((caleeFunction->getName() == "map2check_non_det_long")) {
    this->instrumentNonDetLong(callInst, Ctx);

  } else if ((caleeFunction->getName() == "__VERIFIER_assume")) {
    this->instrumentNonDet(NonDetType::ASSUME, caleeFunction);

  } else if ((caleeFunction->getName() == "__VERIFIER_nondet_ushort")) {
    this->instrumentNonDet(NonDetType::USHORT, caleeFunction);
    this->instrumentNonDetUshort(callInst, Ctx);

  } else if ((caleeFunction->getName() == "map2check_non_det_ushort")) {
    this->instrumentNonDetUshort(callInst, Ctx);

  } else if ((caleeFunction->getName() == "map2check_assume")) {
    // TODO(rafa.sa.xp@gmail.com): Implement method
  }
}

void
NonDetPass::instrumentNonDet(NonDetType nonDetType, Function *caleeFunction) {
  switch (nonDetType) {
    case (NonDetType::INTEGER):
    {
      Twine non_det_int("map2check_non_det_int");
      caleeFunction->setName(non_det_int);
      break;
    }
    case (NonDetType::UNSIGNED):
    {
      Twine non_det_uint("map2check_non_det_uint");
      caleeFunction->setName(non_det_uint);
      break;
    }
    case (NonDetType::CHAR):
    {
      Twine non_det_char("map2check_non_det_char");
      caleeFunction->setName(non_det_char);
      break;
    }
    case (NonDetType::POINTER):
    {
      Twine non_det_pointer("map2check_non_det_pointer");
      caleeFunction->setName(non_det_pointer);
      break;
    }
    case (NonDetType::LONG):
    {
      Twine non_det_long("map2check_non_det_long");
      caleeFunction->setName(non_det_long);
      break;
    }
    case (NonDetType::USHORT):
    {
      Twine non_det_ushort("map2check_non_det_ushort");
      caleeFunction->setName(non_det_ushort);
      break;
    }
    case (NonDetType::ASSUME):
    {
      Twine assume("map2check_assume");
      caleeFunction->setName(assume);
      break;
    }
  }
}

void NonDetPass::instrumentNonDetInt(CallInst* callInst, LLVMContext* Ctx) {
    auto j = this->currentInstruction;
    j++;
    IRBuilder<> builder(BBIteratorToInst(j));
    Value* function_llvm = this->getFunctionNameValue();
    DebugInfo debugInfo(Ctx, callInst);

    Value* args[] = {
        debugInfo.getLineNumberValue(),
        debugInfo.getScopeNumberValue(),
        callInst,
        function_llvm
    };


    Constant* KleeFunction = this->nonDetFunctions->getNonDetIntegerFunction();
    builder.CreateCall(KleeFunction, args);
}

void
NonDetPass::instrumentNonDetUnsigned(CallInst* callInst, LLVMContext* Ctx) {
    auto j = this->currentInstruction;
    j++;
    IRBuilder<> builder(BBIteratorToInst(j));
    Value* function_llvm = this->getFunctionNameValue();
    DebugInfo debugInfo(Ctx, callInst);

    Value* args[] = {
        debugInfo.getLineNumberValue(),
        debugInfo.getScopeNumberValue(),
        callInst,
        function_llvm
    };


    Constant* KleeFunction = this->nonDetFunctions->getNonDetUnsignedFunction();
    builder.CreateCall(KleeFunction, args);
}

void NonDetPass::instrumentNonDetChar(CallInst* callInst, LLVMContext* Ctx) {

    auto j = this->currentInstruction;
    j++;
    IRBuilder<> builder(BBIteratorToInst(j));
    Value* function_llvm = this->getFunctionNameValue();
    Twine bitcast("map2check_klee_char_cast");
    Value* charCast = CastInst::CreateIntegerCast(callInst, Type::getInt32Ty(*Ctx), false, bitcast, BBIteratorToInst(j));
    DebugInfo debugInfo(Ctx, callInst);

    Value* args[] = {
        debugInfo.getLineNumberValue(),
        debugInfo.getScopeNumberValue(),
        // callInst,
        charCast,
        function_llvm
    };

    Constant* KleeFunction = this->nonDetFunctions->getNonDetCharFunction();
    builder.CreateCall(KleeFunction, args);
}

void NonDetPass::instrumentNonDetPointer(CallInst* callInst, LLVMContext* Ctx) {

    auto j = this->currentInstruction;
    j++;
    IRBuilder<> builder(BBIteratorToInst(j));
    Value* function_llvm = this->getFunctionNameValue();
    Twine bitcast("map2check_klee_pointer_cast");
    Value* charCast = CastInst::CreateIntegerCast(callInst, Type::getInt32Ty(*Ctx), false, bitcast, BBIteratorToInst(j));
    DebugInfo debugInfo(Ctx, callInst);

    Value* args[] = {
        debugInfo.getLineNumberValue(),
        debugInfo.getScopeNumberValue(),
        // callInst,
        charCast,
        function_llvm
    };

    Constant* KleeFunction = this->nonDetFunctions->getNonDetPointerFunction();
    builder.CreateCall(KleeFunction, args);
}

void NonDetPass::instrumentNonDetUshort(CallInst* callInst, LLVMContext* Ctx) {

    auto j = this->currentInstruction;
    j++;
    IRBuilder<> builder(BBIteratorToInst(j));
    Value* function_llvm = this->getFunctionNameValue();
    Twine bitcast("map2check_klee_ushort_cast");
    Value* charCast = CastInst::CreateIntegerCast(callInst, Type::getInt32Ty(*Ctx), false, bitcast, BBIteratorToInst(j));
    DebugInfo debugInfo(Ctx, callInst);

    Value* args[] = {
        debugInfo.getLineNumberValue(),
        debugInfo.getScopeNumberValue(),
        // callInst,
        charCast,
        function_llvm
    };

    Constant* KleeFunction = this->nonDetFunctions->getNonDetUshortFunction();
    builder.CreateCall(KleeFunction, args);
}

void NonDetPass::instrumentNonDetLong(CallInst* callInst, LLVMContext* Ctx) {

    auto j = this->currentInstruction;
    j++;
    IRBuilder<> builder(BBIteratorToInst(j));
    Value* function_llvm = this->getFunctionNameValue();
    Twine bitcast("map2check_klee_long_cast");
    Value* charCast = CastInst::CreateIntegerCast(callInst, Type::getInt32Ty(*Ctx), false, bitcast, BBIteratorToInst(j));
    DebugInfo debugInfo(Ctx, callInst);
    Value* args[] = {
        debugInfo.getLineNumberValue(),
        debugInfo.getScopeNumberValue(),
        // callInst,
        charCast,
        function_llvm
    };

    Constant* KleeFunction = this->nonDetFunctions->getNonDetLongFunction();
    builder.CreateCall(KleeFunction, args);
}

char NonDetPass::ID = 1;
static RegisterPass<NonDetPass> X("non_det", "Adds klee calls for non deterministic methods (from SVCOMP)");


