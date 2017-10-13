#include "TrackBasicBlockPass.hpp"

bool TrackBasicBlockPass::runOnFunction(Function &F) {
    this->Ctx = &F.getContext();
    this->currentFunction = &F;      
    
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
			errs() << this->numLineBlk << "\n";			

		}else{
			//empty line in the source code
			DebugInfo debugInfoLa(this->Ctx, (Instruction*)this->st_lastBlockInst);
			this->numLineBlk = debugInfoLa.getLineNumberInt();
			errs() << this->numLineBlk << "\n";			
		}
	}		
    
}

void TrackBasicBlockPass::instrumentLastInstBB(BasicBlock::iterator& iT)
{	
	auto j = iT;
	j++;
	Twine track_bb("map2check_track_bb");
	Function *caleeFunction;
	caleeFunction->setName(track_bb);
	
	IRBuilder<> builder((Instruction*)j);
	Value* function_llvm = this->getFunctionNameValue();
	DebugInfo debugInfo(this->Ctx, (Instruction*)iT);

	Value* args[] = {
		debugInfo.getLineNumberValue(),	
		function_llvm
	};
		
	builder.CreateCall(this->libraryFunctions->getTrackBBFunction(), args);
}

char TrackBasicBlockPass::ID = 12;
static RegisterPass<TrackBasicBlockPass> X("track_basic_block", "Trancking Basic Block in the symbolic execution");




