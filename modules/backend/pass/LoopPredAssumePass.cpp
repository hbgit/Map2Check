#include "LoopPredAssumePass.hpp"

namespace {
inline Instruction* BBIteratorToInst(BasicBlock::iterator i) {
  Instruction* pointer = reinterpret_cast<Instruction*>(&*i);
  return pointer;
}
}  // namespace

void LoopPredAssumePass::getConditionInLoop(Loop* L) {
	//Loop::block_iterator bb;
	BasicBlock* header = L->getHeader();

	if (BranchInst *bi = dyn_cast<BranchInst>(header->getTerminator())) {

		//errs() << *bi->getCondition() << "\n";                
		Value *loopCond = bi->getCondition();                
		//errs() << *loopCond << "\n";

		CmpInst *cmpInst = dyn_cast<CmpInst>(&*loopCond);                

		if( bi->getNumSuccessors() > 0){
			BasicBlock* succ_cond_bb = bi->getSuccessor(0);
			BasicBlock::iterator iT = --succ_cond_bb->end();

			auto *new_inst = cmpInst->clone();
			auto *inst_pos = dyn_cast<Instruction>(&*--succ_cond_bb->end());
			errs() << *inst_pos << "\n";
			new_inst->insertBefore(inst_pos);
			
			CmpInst *new_cmpInst = dyn_cast<CmpInst>(&*new_inst);
			new_cmpInst->setPredicate(new_cmpInst->getInversePredicate());

			Value *new_loop_cond = dyn_cast<Value>(&*new_cmpInst); // Convert CmpInst to Value;
			
			IRBuilder<> builder(BBIteratorToInst(iT));                                        

			//e.g., call function map2check_assume(a < b)
			this->map2check_assume = this->currentFunction->getParent()->getOrInsertFunction(
									 "map2check_assume",
									Type::getVoidTy(this->currentFunction->getContext()),
									Type::getInt1Ty(this->currentFunction->getContext())
									);

			Value* args[] = {new_loop_cond};                    
			builder.CreateCall(this->map2check_assume, args);
		}
	}

}

bool LoopPredAssumePass::runOnFunction(Function& F) {
  this->Ctx = &F.getContext();
  this->currentFunction = &F;

  // e.g., call function map2check_assume(a < b)
  this->map2check_assume =
      this->currentFunction->getParent()->getOrInsertFunction(
          "__VERIFIER_assume", Type::getVoidTy(*this->Ctx),
          Type::getInt1Ty(*this->Ctx));

  LoopInfo& LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();

  for (LoopInfo::iterator i = LI.begin(), e = LI.end(); i != e; ++i)
    getConditionInLoop(*i);

  return (false);
}

char LoopPredAssumePass::ID = 13;
static RegisterPass<LoopPredAssumePass> X(
    "loop_predicate_assume",
    "It takes the loop predicate and force it as loop post-cond assume");
