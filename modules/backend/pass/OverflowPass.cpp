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
		//i->dump();
		
		if(auto* cI = dyn_cast<CallInst>(&*i))        
        {   			
			Value* v = cI->getCalledValue();	
			Function* caleeFunction = dyn_cast<Function>(v->stripPointerCasts());						
            if(caleeFunction->getName() == "__VERIFIER_nondet_uint" ||
               caleeFunction->getName() == "map2check_non_det_uint")
            {
                DebugInfo debugInfoCi(this->Ctx, cI);                         
                //errs() << debugInfoCi.getLineNumberInt() << "==================\n";
                
                //cI->dump();
                i++;
                i++; //jump klee line
                //i->dump();
                if (StoreInst* storeI = dyn_cast<StoreInst>(&*i)) {
					//errs() << *storeI->getOperand(1) << "--\n";
					this->storeInstWithUint.push_back(&*storeI->getOperand(1));
				}
                
            }

        } 
	}
	
	//Handle with the variable assignment v1 = i by a nondet UINT
	//Identify all store inst, then read each load inst from operand ZERO
	//to compare if the variable was identified as nondet UINT in this->storeInstWithUint
	for (BasicBlock::iterator i = B.begin(), e = B.end(); i != e; ++i)
	{
		if (StoreInst* storeI = dyn_cast<StoreInst>(&*i)) 
		{
			//Value* vst_1 = storeI->getOperand(1);					

			if (LoadInst* loadI = dyn_cast<LoadInst>(&*storeI->getOperand(0))) 
			{				
				Value* vload = &*loadI->getPointerOperand();				
								
				std::vector<Value*>::iterator iT;
				iT =  std::find(this->storeInstWithUint.begin(), 
								this->storeInstWithUint.end(), 
								vload); 
				
				if ( iT != this->storeInstWithUint.end() )
				{				
					//DebugInfo debugInfoCi(this->Ctx, loadI);                         
					//errs() << debugInfoCi.getLineNumberInt() << ">>>>>>>>>>>> \n";	
					//loadI->dump();
					//this->listLineNumUint.push_back(debugInfoCi.getLineNumberInt());
					this->storeInstWithUint.push_back(&*storeI->getOperand(1));
				}
			}
			
		}
	}
	
	
	//Read each load inst to identify if it has a nondet uint value
	for (BasicBlock::iterator i = B.begin(), e = B.end(); i != e; ++i)
	{
		
		if (LoadInst* loadI = dyn_cast<LoadInst>(&*i)) 
		{
			
			//errs() << "-------------------- \n";
			//loadI->dump();
			DebugInfo debugInfoCi(this->Ctx, loadI);                         
			//errs() << debugInfoCi.getLineNumberInt() << "************* \n";		
			
			
			/**
			std::vector<Value*>::iterator iter;			
			for(iter = this->storeInstWithUint.begin();
				iter != this->storeInstWithUint.end();
				++iter)
			{
				Value* vl = *iter;
				errs() << *vl << "<<<<<<<<<<<<< \n";
			}**/
				   
			
			Value* vload = &*loadI->getPointerOperand();					
			
			std::vector<Value*>::iterator iT;
			iT =  std::find(this->storeInstWithUint.begin(), 
							this->storeInstWithUint.end(), 
							vload); 
			
            if ( iT != this->storeInstWithUint.end() )
            {				
				DebugInfo debugInfoCi(this->Ctx, loadI);                         
				//errs() << debugInfoCi.getLineNumberInt() << "************* \n";	
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
    
  
	SmallVector<std::pair<unsigned, MDNode *>, 4> MDs;
	F.getAllMetadata(MDs);
	for (auto &MD : MDs) {
	  if (MDNode *N = MD.second) {
		if (auto *subProgram = dyn_cast<DISubprogram>(N)) {
		  errs() << subProgram->getLine() << "+++++++++ \n";
		}
	  }
	}
    
  
    
  for(auto& B:F)
  {
	  this->listAllUintAssig(B);
	  
		for(auto& I:B){
			SmallVector<std::pair<unsigned, MDNode *>, 4> MDs;
			I.getAllMetadata(MDs);
			for (auto &MD : MDs) {
				errs() << MD.first << "++++\n";
			  if (MDNode *N = MD.second) {
				
				errs() << *N << "++++\n";
				//if (auto *subProgram = dyn_cast<DISubprogram>(N)) {
				  //errs() << subProgram->getLine() << "+++++++++ \n";
				//}
				
			  }
			}
		}
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
    Value* secondOperand = binOp->getOperand(1);
	currentInstruction++;	
	  
	std::vector<int>::const_iterator iT;
	iT =  std::find(this->listLineNumUint.begin(), 
					this->listLineNumUint.end(), 
					debugInfo.getLineNumberInt()); 
	
	//errs() << debugInfo.getLineNumberInt() << "=============\n";
	
	bool flagCallNondetInt = false;
	// This is a trigger. TODO: Remove this and replace by a 
	// properly identification of variable type
	if(CallInst* callI = dyn_cast<CallInst>(&*firstOperand))
	{
		flagCallNondetInt = true;
	}else if(CallInst* callI = dyn_cast<CallInst>(&*secondOperand))
	{
		flagCallNondetInt = true;
	}
	
	if ( iT != this->listLineNumUint.end() )
	{
		this->isUnitAssigment = true;
	}else if(flagCallNondetInt)
	{
		this->isUnitAssigment = true;
	}else{
		this->isUnitAssigment = false;
	}
	
	//errs() << this->isUnitAssigment << "??????\n";
	//errs() << binOp->getOpcode() << "\n";
	  
	switch(binOp->getOpcode()) {
	case(Instruction::Add):
	  if(this->isUnitAssigment)
	  {
		//errs() << "getOverflowAddUint \n";
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
		//errs() << "getOverflowSubUint \n";
		instrumentedFunction = this->operationsFunctions->getOverflowSubUint();
	  }else{
		instrumentedFunction = this->operationsFunctions->getOverflowSub();
	  }
	  break;
	case(Instruction::FSub):
	  
	  break;
	case(Instruction::Mul):
		//errs() << "MULT \n";
		if(this->isUnitAssigment)
		{
			instrumentedFunction = this->operationsFunctions->getOverflowMulUint();
		}else{
			instrumentedFunction = this->operationsFunctions->getOverflowMul();
		}
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
