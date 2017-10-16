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
    
    if(!this->initializedFunctionName) {
	  IRBuilder<> builder((Instruction*)&*this->st_lastBlockInst);
	  this->functionName = builder
			  .CreateGlobalStringPtr(this->currentFunction->getName());
	  this->initializedFunctionName = true;
    }
			
	if(auto* tI = dyn_cast<TerminatorInst>(&*this->st_lastBlockInst))
	{
		if(tI->getOpcodeName() == "br")
		{
			--this->st_lastBlockInst;            
			DebugInfo debugInfoLa(this->Ctx, (Instruction*)this->st_lastBlockInst);            
			this->numLineBlk = debugInfoLa.getLineNumberInt();            
			errs() << this->numLineBlk << " 1 \n";	
			this->instrumentLastInstBB(this->st_lastBlockInst);		

		}else{
			//empty line in the source code
			DebugInfo debugInfoLa(this->Ctx, (Instruction*)this->st_lastBlockInst);
			this->numLineBlk = debugInfoLa.getLineNumberInt();
			errs() << this->numLineBlk << " 2 \n";			
			this->instrumentLastInstBB(this->st_lastBlockInst);
		}
	}		
    
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
	

	Value* args[] = {
		debugInfo.getLineNumberValue(),	
		function_llvm
	};
		
	builder.CreateCall(this->libraryFunctions->getTrackBBFunction(), args);
	//errs() << "inst 3 \n";
}

char TrackBasicBlockPass::ID = 12;
static RegisterPass<TrackBasicBlockPass> X("track_basic_block", "Trancking Basic Block in the symbolic execution");




