#include "TrackBasicBlockPass.hpp"

bool TrackBasicBlockPass::runOnFunction(Function &F) {
    this->Ctx = &F.getContext();
    this->currentFunction = &F;      
    this->libraryFunctions =  make_unique<LibraryFunctions>(&F, &F.getContext());
    for(auto& B: F)
    {

        this->runOnBasicBlock(B, this->Ctx);       

    }

    return true;
}

void TrackBasicBlockPass::runOnBasicBlock(BasicBlock& B, LLVMContext* Ctx) 
{    
        
    this->st_lastBlockInst = --B.end(); // -- is necessary to avoid the pointer to the next block        
    
    /**if(!this->initializedFunctionName) {
	  IRBuilder<> builder((Instruction*)&*this->st_lastBlockInst);
	  this->functionName = builder
			  .CreateGlobalStringPtr(this->currentFunction->getName());
	  this->initializedFunctionName = true;
    }**/
    
	IRBuilder<> builder((Instruction*)&*this->st_lastBlockInst);
	this->functionName = builder.CreateGlobalStringPtr(this->currentFunction->getName());
		
	//errs() << this->currentFunction->getName() << "\n";
	//errs() << this->st_lastBlockInst->getOpcodeName() << "\n";
	//DebugInfo debugInfoLa(this->Ctx, (Instruction*)this->st_lastBlockInst);
	//errs() << debugInfoLa.getLineNumberInt() << "\n";
			
	if(auto* tI = dyn_cast<TerminatorInst>(&*this->st_lastBlockInst))
	{
		
		if(tI->getOpcodeName() == "br")
		{
			if(B.size() > 1){
				--this->st_lastBlockInst; 
				DebugInfo debugInfoLa(this->Ctx, (Instruction*)this->st_lastBlockInst);            
				this->numLineBlk_ori = debugInfoLa.getLineNumberInt();
				bool flagAssume = false;
				//Checking if the instruction is an ASSUME to skip			
				if(this->checkInstBBIsAssume(this->st_lastBlockInst))
				{
					flagAssume = true;
				}
											
				while(flagAssume){						
					--this->st_lastBlockInst;	
					DebugInfo debugInfoAa(this->Ctx, (Instruction*)this->st_lastBlockInst);            
					this->numLineBlk_AA = debugInfoAa.getLineNumberInt();			
					//errs() << this->numLineBlk_AA << "  " << this->numLineBlk_ori << "\n";
					if(this->numLineBlk_AA == this->numLineBlk_ori)
					{						
						flagAssume = true;
					}else{
						flagAssume = false;
					}
					
				}								
				
				
				//errs() << this->numLineBlk_AA << " 1 \n";	
				this->instrumentLastInstBB(this->st_lastBlockInst);		
			}

		}else{						
			
			DebugInfo debugInfoLa(this->Ctx, (Instruction*)this->st_lastBlockInst);            
			this->numLineBlk_ori = debugInfoLa.getLineNumberInt();
			bool flagAssume = false;
			//Checking if the instruction is an ASSUME to skip			
			if(this->checkInstBBIsAssume(this->st_lastBlockInst))
			{
				flagAssume = true;
			}
										
			while(flagAssume){						
				--this->st_lastBlockInst;	
				DebugInfo debugInfoAa(this->Ctx, (Instruction*)this->st_lastBlockInst);            
				this->numLineBlk_AA = debugInfoAa.getLineNumberInt();			
				//errs() << this->numLineBlk_AA << "  " << this->numLineBlk_ori << "\n";
				if(this->numLineBlk_AA == this->numLineBlk_ori)
				{					
					flagAssume = true;
				}else{
					flagAssume = false;
				}
				
			}
					
			//this->numLineBlk_AA = debugInfoLa.getLineNumberInt();
			
			// To avoid empty lines, e.g., only with "}"
			bool flagEmpty = false;			
			DebugInfo debugInfoAaEmpty(this->Ctx, (Instruction*)this->st_lastBlockInst);
			if(this->sourceCodeHelper->getLine(debugInfoAaEmpty.getLineNumberInt()).empty())
			{
				flagEmpty = true;
			}
			
			while(flagEmpty)
            {
				--this->st_lastBlockInst;
				DebugInfo debugInfoAaEmptyW(this->Ctx, (Instruction*)this->st_lastBlockInst);	
				if(this->sourceCodeHelper->getLine(debugInfoAaEmptyW.getLineNumberInt()).empty())
				{
					flagEmpty = true;
				}else{
					flagEmpty = false;
				}
			}
			this->instrumentLastInstBB(this->st_lastBlockInst);
		}	
		
	}
    
}

bool TrackBasicBlockPass::checkInstBBIsAssume(BasicBlock::iterator& iT)
{	
	if(auto* cI = dyn_cast<CallInst>((Instruction*)iT))        
	{   
		
		Value* v = cI->getCalledValue();	
		Function* caleeFunction = dyn_cast<Function>(v->stripPointerCasts());								
		
		if(caleeFunction->getName() == "__VERIFIER_assume" ||
		   caleeFunction->getName() == "map2check_assume"  )
		{		
			return true;
		}else{
			return false;
		}		

	}
	return false;
}

void TrackBasicBlockPass::instrumentLastInstBB(BasicBlock::iterator& iT)
{	
	/**auto j = iT;
		
	errs() << iT->getOpcodeName() << "\n";
	if(iT->getOpcodeName() == "ret")
	{
		++j;
	}else{
		--j;
	}**/
	
	//Twine track_bb("map2check_track_bb");
	//Function *caleeFunction;
	//caleeFunction->setName(track_bb);
	//errs() << "inst 1 \n";
	
	IRBuilder<> builder((Instruction*)iT);
	//errs() << "inst 2 \n";
	Value* function_llvm = this->getFunctionNameValue();
	DebugInfo debugInfo(this->Ctx, (Instruction*)iT);
	//errs() << *debugInfo.getLineNumberValue() << "\n";
	//errs() << function_llvm->getName() << "\n";

	Value* args[] = {
		debugInfo.getLineNumberValue(),	
		function_llvm
	};
		
	builder.CreateCall(this->libraryFunctions->getTrackBBFunction(), args);
	//errs() << "inst 3 \n";
}

char TrackBasicBlockPass::ID = 12;
static RegisterPass<TrackBasicBlockPass> X("track_basic_block", "Trancking Basic Block in the symbolic execution");




