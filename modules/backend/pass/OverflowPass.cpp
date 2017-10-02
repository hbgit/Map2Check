#include "OverflowPass.h"

bool OverflowPass::runOnFunction(Function &F) {
  this->operationsFunctions =  make_unique<OperationsFunctions>(&F, &F.getContext());
  Function::iterator functionIterator = F.begin();
  BasicBlock::iterator instructionIterator = functionIterator->begin();

  IRBuilder<> builder((Instruction*)&*instructionIterator);
  this->functionName = builder
    .CreateGlobalStringPtr(F.getName());

  for (Function::iterator bb = F.begin(), e = F.end();
       bb != e; ++bb) {
    for (BasicBlock::iterator i = bb->begin(),
           e = bb->end(); i != e; ++i) {      
      
      if (BinaryOperator* binOp = dyn_cast<BinaryOperator>(&*i)) {
	BasicBlock::iterator currentInstruction = i;
	IRBuilder<> builder((Instruction*)currentInstruction);
	Value* function_llvm = this->getFunctionNameValue();
	Twine bitcast("map2check_pointer_cast");
	DebugInfo debugInfo(&F.getContext(), binOp);

	Constant* instrumentedFunction = NULL;

	Value* firstOperand = binOp->getOperand(0);
        Value* secondOperand = binOp->getOperand(1);
	currentInstruction++;
	// Value* firstOperandCast = CastInst::CreatePointerCast
	  // (firstOperand,
	  //  Type::getInt8PtrTy(F.getContext()),
	  //  bitcast,(Instruction*) currentInstruction);
	switch(binOp->getOpcode()) {
	case(Instruction::Add):
	  instrumentedFunction = this->operationsFunctions->getOverflowAdd();
	  break;
	case(Instruction::FAdd):
	  
	  break;
	case(Instruction::Sub):
      instrumentedFunction = this->operationsFunctions->getOverflowSub();
	  break;
	case(Instruction::FSub):
	  
	  break;
	case(Instruction::Mul):
        instrumentedFunction = this->operationsFunctions->getOverflowMul();
	  break;
	case(Instruction::FMul):
	  
	  break;
	case(Instruction::UDiv):
	  
	  break;
	case(Instruction::SDiv):
        instrumentedFunction = this->operationsFunctions->getOverflowSDiv();
	  break;
	case(Instruction::FDiv):
	  
	  break;
	case(Instruction::URem):
	  
	  break;
	case(Instruction::SRem):
	  
	  break;
	case(Instruction::FRem):
	  
	  break;
	case(Instruction::Shl):
	  
	  break;
	case(Instruction::LShr):
	  
	  break;
	case(Instruction::AShr):
	  
	  break;
	case(Instruction::And):
	  
	  break;
	case(Instruction::Or):
	  
	  break;
	case(Instruction::Xor):
	  
	  break;
	case(Instruction::BinaryOpsEnd):
	  
	  break;
	}

	if (instrumentedFunction != NULL) {
	  Value* args[] = {
	    firstOperand,
	    secondOperand,
	    debugInfo.getLineNumberValue(),
	    debugInfo.getScopeNumberValue(),
	    functionName
	  };
	  builder.CreateCall(instrumentedFunction, args);
	  
	}
	else {
	  //errs() << binOp->getOpcodeName() <<" not supported\n";
	}
      }
    }
  }
  return true;
}

char OverflowPass::ID = 10;
static RegisterPass<OverflowPass> X("check_overflow", "Validate overflow on signed integer dynamic operations tracking");
