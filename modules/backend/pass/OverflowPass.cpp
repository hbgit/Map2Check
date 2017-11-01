#include "OverflowPass.h"

void OverflowPass::hasNonDetUint(Instruction* I)
{
	//I->dump();
	DebugInfo debugInfo(this->Ctx, I);
	//errs() << *debugInfo.getLineNumberValue() << "================\n";
}


void OverflowPass::listAllUintAssig(BasicBlock &B)
{
	for (BasicBlock::iterator i = B.begin(), e = B.end(); i != e; ++i)
	{
		if(auto* cI = dyn_cast<CallInst>(&*i))        
        {   			
			Value* v = cI->getCalledValue();	
			Function* caleeFunction = dyn_cast<Function>(v->stripPointerCasts());						
            if(caleeFunction->getName() == "__VERIFIER_nondet_uint" ||
               caleeFunction->getName() == "map2check_non_det_uint")
            {
                DebugInfo debugInfoCi(this->Ctx, cI);                         
                errs() << debugInfoCi.getLineNumberInt() << "==================\n";
                
                //cI->dump();
                i++;
                i++; //jump klee call
                //i->dump();
                if (StoreInst* storeI = dyn_cast<StoreInst>(&*i)) {
					//errs() << storeI->getOperand(1) << "\n";
					this->storeInstWithUint.push_back(storeI->getOperand(1));
				}
                
            }

        } 
	}
	
	for (BasicBlock::iterator i = B.begin(), e = B.end(); i != e; ++i)
	{
		
		if (LoadInst* loadI = dyn_cast<LoadInst>(&*i)) 
		{
			
			std::vector<Value*>::const_iterator k = this->storeInstWithUint.begin();
			while (k != this->storeInstWithUint.end ()){
				if(Value* v = dyn_cast<Value>(*k)){
					errs() << v->getName() << "\n";					
					errs() << *loadI->getOperand(1) << "\n"; //TODO: How get value getName()
				}
			   
			   ++ k;
		   }
			
			std::vector<Value*>::const_iterator iT;
			iT =  std::find(this->storeInstWithUint.begin(), 
							this->storeInstWithUint.end(), 
							loadI->getOperand(1)); 
			
            if ( iT != this->storeInstWithUint.end() )
            {
				//loadI->dump();
				DebugInfo debugInfoCi(this->Ctx, loadI);                         
				errs() << debugInfoCi.getLineNumberInt() << "<<<<<<<<<<<<<<,,\n";	
				this->listLineNumUint.push_back(debugInfoCi.getLineNumberInt());
			}
			
					
		}
	}
	
}

bool OverflowPass::runOnFunction(Function &F) {
  this->operationsFunctions =  make_unique<OperationsFunctions>(&F, &F.getContext());
  Function::iterator functionIterator = F.begin();
  BasicBlock::iterator instructionIterator = functionIterator->begin();
  
  this->Ctx = &F.getContext();

  IRBuilder<> builder((Instruction*)&*instructionIterator);
  this->functionName = builder
    .CreateGlobalStringPtr(F.getName());
    
  for(auto& B:F)
  {
	  this->listAllUintAssig(B);
  }

  for (Function::iterator bb = F.begin(), e = F.end();
       bb != e; ++bb) {	
		   
    for (BasicBlock::iterator i = bb->begin(),
           e = bb->end(); i != e; ++i) {      
    
    //i->dump();      
    
    
    if (BinaryOperator* binOp = dyn_cast<BinaryOperator>(&*i)) {
	BasicBlock::iterator currentInstruction = i;
	
	
	IRBuilder<> builder((Instruction*)currentInstruction);
	Value* function_llvm = this->getFunctionNameValue();
	Twine bitcast("map2check_pointer_cast");
	DebugInfo debugInfo(&F.getContext(), binOp);

	Constant* instrumentedFunction = NULL;

	Value* firstOperand = binOp->getOperand(0);
	/**
	if(isa<LoadInst>(firstOperand)) {
		LoadInst *LD0 = cast<LoadInst>(firstOperand);		
		LD0->dump();
		Instruction *iO = dyn_cast<Instruction>(LD0->getPointerOperand());
		//iO->dump();
		this->hasNonDetUint(iO);

	} 	**/
	
    Value* secondOperand = binOp->getOperand(1);
	currentInstruction++;
	// Value* firstOperandCast = CastInst::CreatePointerCast
	  // (firstOperand,
	  //  Type::getInt8PtrTy(F.getContext()),
	  //  bitcast,(Instruction*) currentInstruction);
	  
	std::vector<int>::const_iterator iT;
	iT =  std::find(this->listLineNumUint.begin(), 
					this->listLineNumUint.end(), 
					debugInfo.getLineNumberInt()); 
	
	errs() << debugInfo.getLineNumberInt() << "=============\n";
	
	if ( iT != this->listLineNumUint.end() )
	{
		this->isUnitAssigment = true;
	}else{
		this->isUnitAssigment = false;
	}
	
	errs() << this->isUnitAssigment << "??????\n";
	  
	switch(binOp->getOpcode()) {
	case(Instruction::Add):
	  if(this->isUnitAssigment)
	  {
		instrumentedFunction = this->operationsFunctions->getOverflowAddUint();
	  }else{
		instrumentedFunction = this->operationsFunctions->getOverflowAdd();
	  }
	  break;
	case(Instruction::FAdd):
	  
	  break;
	case(Instruction::Sub):
	  if(this->isUnitAssigment)
	  {
		instrumentedFunction = this->operationsFunctions->getOverflowSubUint();
	  }else{
		instrumentedFunction = this->operationsFunctions->getOverflowSub();
	  }
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
	  
	  //errs() << *firstOperand << "\n";
	  //errs() << *secondOperand << "\n";
		
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
