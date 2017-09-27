#include "GenerateAutomataTruePass.h"

using namespace llvm;

bool GenerateAutomataTruePass::runOnFunction(Function &F) {
    this->Ctx = &F.getContext();
    this->currentFunction = &F;   
    
    for (auto& B : F) {
		for (auto& I : B) {     			
								
			//if(ICmpInst *CI = dyn_cast<ICmpInst>(I++)) {
			if(auto* iCmpInst = dyn_cast<ICmpInst>(&I))
			{
			//if ( strcmp(I.getOperand(0)->getName().str().c_str(), "icmp") == 0 ){					
				I.dump();				
				errs() << " <<< \n";				
				
				errs() << iCmpInst->getOpcodeName() << '\t'; 
				errs() << iCmpInst->getPredicate() << '\t'; 				
			
			}
		}    
   
	}
	return true;
}

char GenerateAutomataTruePass::ID = 1;
static RegisterPass<GenerateAutomataTruePass> X("generate_automata_true", "Generate Automata True", false, false);
//

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
/**
static void RegisterGenerateAutomataTruePass(const PassManagerBuilder &,
                                             legacy::PassManagerBase &PM, 
                                             std::string target_function) {
    PM.add(new GenerateAutomataTruePass(target_function));
}

static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 RegisterGenerateAutomataTruePass);
**/




