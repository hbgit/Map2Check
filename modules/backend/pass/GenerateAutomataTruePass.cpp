#include "GenerateAutomataTruePass.h"

bool GenerateAutomataTruePass::runOnFunction(Function &F) {
    this->Ctx = &F.getContext();
    this->currentFunction = &F;  

    errs() << "========== Function: " << this->currentFunction->getName() << "\n";
    if(this->currentFunction->getName() == "main")
    {
        errs() << ">>> Entry Point \n";
    }
    //for (auto& B : F) {
    // errs() << B.getName();
    /**
      for (auto& I : B) {     			

    //I.dump();

    if(auto* iCmpInst = dyn_cast<ICmpInst>(&I))
    {
    //if ( strcmp(I.getOperand(0)->getName().str().c_str(), "icmp") == 0 ){					
    I.dump();				
    errs() << " <<< \n";				

    errs() << iCmpInst->getOpcodeName() << '\t'; 
    errs() << iCmpInst->getPredicate() << '\t'; 				

    }
    }  **/  

    // }
    //
    //Function::iterator functionIterator = F.begin();
    //BasicBlock::iterator instructionIterator = functionIterator->begin();

    //IRBuilder<> builder((Instruction*)&*instructionIterator);
    //errs() << F.getEntryBlock().getName() << "\n";
    for(auto& B: F)
    {

        //BasicBlock basicB = i;  
        this->runOnBasicBlock(B, this->Ctx); 
        /**
          if (CallInst* callInst = dyn_cast<CallInst>(&*i)) {
          currentInstruction = i;
          this->runOnCallInstruction(callInst, &F.getContext());
          }**/
    }
    //}
    errs() << "\n";

    return true;
}


void GenerateAutomataTruePass::runOnBasicBlock(BasicBlock& B, LLVMContext* Ctx) 
{
    //B.dump();    
    //Instruction I = dyn_cast<Instruction>(&B.end());
    // Get the first and the last instruction of the BasicBlock 
    // Check if the Block has size greater then 1, otherwise the last instruction 
    // will be the first one 

    //BasicBlock::iterator instFromBB;
    this->firstBlockInst = B.begin();
    this->lastBlockInst = --B.end(); // -- is necessary to avoid the pointer to the next block
    bool enableDataBlk = false;
    if(!B.hasName()){
        B.printAsOperand(errs(), false);
        StringRef bbName(B.getName());
        this->labelBasicBlock = bbName;
        errs() << "<<< Basic Block Name" << this->labelBasicBlock << "\n";
    }else{
        this->labelBasicBlock = B.getName();
        errs() << "<<< Basic Block Name" << this->labelBasicBlock << "\n";
    }


    //errs() << this->checkBBHasLError(B) << "\n";
    if(!this->checkBBHasLError(B))
    {

        // TODO: Create a method to get info from branches in the block for condition-true and false     
        this->isBranchCond(B);

        if(B.size() > 1)
        {
            errs() << "First InstBB: " << this->firstBlockInst->getOpcodeName() << "\n";
            //Checking if the last instruction in the basic block is a br instruction,i.e., 
            //this br instrucion 
            if(auto* tI = dyn_cast<TerminatorInst>(&*this->lastBlockInst))
            {
                if(tI->getOpcodeName() == "br")
                {
                    --this->lastBlockInst;
                    errs() << "Last  InstBB: " << this->lastBlockInst->getOpcodeName() << "\n";                
                }else{
                    errs() << "Last  InstBB: " << this->lastBlockInst->getOpcodeName() << "\n";                
                }
            }
            enableDataBlk = true;

        }else{
            //In case this unique instruction be a "br" then we remove this basic block 
            if(auto* tI = dyn_cast<TerminatorInst>(&*this->lastBlockInst))
            {
                if(tI->getOpcodeName() != "br")
                {
                    errs() << "First and Last InstBB: "<< this->firstBlockInst->getOpcodeName() << "\n";        
                    this->lastBlockInst = this->firstBlockInst;
                    enableDataBlk = true;
                }
            }

        }
    }

    if(enableDataBlk)
    {
        //this->visitInstruction(i);
        DebugInfo debugInfo(Ctx, (Instruction*)this->lastBlockInst);
        //errs() << "Line number to the next block: " << *debugInfo.getLineNumberValue() <<"\n";
        errs() << ">>>Line number to the next block: " << debugInfo.getLineNumberInt() <<"\n";
        std::cout << "---This is a block divider--- block with: " << B.size() << " instructions" << std::endl;    
    }
}

// Identify if the block has a branch and define the condition to true and false
bool GenerateAutomataTruePass::isBranchCond(BasicBlock& B)
{
    //errs() << bbName << "\n";

    for( auto& I : B )
    {        
        
        if(auto* bI = dyn_cast<ICmpInst>(&I))
        {
            bI->dump();
            errs() << bI->getPredicate() << "\n";
            errs() << *bI->getOperand(0) << "\n";
            errs() << *bI->getOperand(1) << "\n";
            DebugInfo debugInfoBi(this->Ctx, bI);
            errs() << debugInfoBi.getVarName() << " <> " << debugInfoBi.getLineNumberInt() << "\n";
            errs() << this->cprogram_path << "\n";
            errs() << this->sourceCodeHelper->getLine(debugInfoBi.getLineNumberInt()) << "\n";
        }


        if(auto* tI = dyn_cast<TerminatorInst>(&I))
        {
            if(tI->getOpcodeName() == "br")
            {
                //errs() << tI->getSuccessor(1)->getValueName() << "\n";
                //errs() << tI->successors().begin()->getValueName() << "\n";
                    
                errs() << tI->getOpcodeName() << ":::::::::::::::: \n";
                tI->dump();
                if(tI->getNumSuccessors() > 1)
                {
                    BasicBlock::iterator trueCond = tI->getSuccessor(0)->begin();
                    DebugInfo debugInfoT(this->Ctx, (Instruction*)trueCond);
                    errs() << "True Cond in:" << debugInfoT.getLineNumberInt() << "\n";
                    
                    BasicBlock::iterator falseCond = tI->getSuccessor(1)->begin();
                    DebugInfo debugInfoF(this->Ctx, (Instruction*)falseCond);
                    errs() << "False Cond in:" << debugInfoF.getLineNumberInt() << "\n";

                }

            }
            /**
            if(tI->getOpcodeName() == "br")
            {
            }**/
        }
 
        //errs() << I.getOpcodeName() << "\n";
    }

    return false;
}

//To identify a block with a error location by __VERIFIER_error call function
bool GenerateAutomataTruePass::checkBBHasLError(BasicBlock& nowB)
{
    for( auto& I : nowB )
    {
        if(CallInst* callInst = dyn_cast<CallInst>(&I))
        {
            //errs() << callInst->getCalledFunction()->getName() << "\n";
            if(callInst->getCalledFunction()->getName() ==  "__VERIFIER_error")
            {
                return true;
            }
        }
    }
    //std::cout << i.begin << "\n";
    //std::cout << i->getOpcodeName(i->getOpcode()) << std::endl;
    return false;
}

char GenerateAutomataTruePass::ID = 5;
static RegisterPass<GenerateAutomataTruePass> X("generate_automata_true", "Generate Automata True");




