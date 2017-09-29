#include "OverflowPass.h"

bool OverflowPass::runOnFunction(Function &F) {

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
                errs() << binOp->getNumOperands() << "\n";
                errs() << binOp->getOpcodeName() << "\n";

            }
        }
      }
      return true;
}

char OverflowPass::ID = 10;
static RegisterPass<OverflowPass> X("check_overflow", "Validate overflow on signed integer dynamic operations tracking");
