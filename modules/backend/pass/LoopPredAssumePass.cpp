#include "LoopPredAssumePass.hpp"

namespace {
inline Instruction* BBIteratorToInst(BasicBlock::iterator i) {
  Instruction* pointer = reinterpret_cast<Instruction*>(&*i);
  return pointer;
}
}  // namespace


void LoopPredAssumePass::getConditionInLoop(Loop *L){
    Loop::block_iterator bb;
    BasicBlock* header = L->getHeader();

    if (BranchInst *bi = dyn_cast<BranchInst>(header->getTerminator())) {
    
        Value *loopCond = bi->getCondition();

        if( bi->getNumSuccessors() > 0){
            BasicBlock* succ_cond_bb = bi->getSuccessor(0);
            BasicBlock::iterator iT = --succ_cond_bb->end();
            
            IRBuilder<> builder(BBIteratorToInst(iT));                    
            
            Value* args[] = {loopCond};                    
            builder.CreateCall(this->map2check_assume, args);

        }
    }    
}

bool LoopPredAssumePass::runOnFunction(Function& F) {
  this->Ctx = &F.getContext();
  this->currentFunction = &F;  

  //e.g., call function map2check_assume(a < b)
  this->map2check_assume = this->currentFunction->getParent()->getOrInsertFunction(
                           "__VERIFIER_assume",
                           Type::getVoidTy(*this->Ctx),                                    
                           Type::getInt1Ty(*this->Ctx), NULL
                           );

  LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
  
  for(LoopInfo::iterator i = LI.begin(), e = LI.end(); i != e; ++i)
    getConditionInLoop(*i);

  return(false);
}


char LoopPredAssumePass::ID = 13;
static RegisterPass<LoopPredAssumePass> X(
    "loop_priedicate_assume", "It takes the loop predicate and force it as loop post-cond assume");
