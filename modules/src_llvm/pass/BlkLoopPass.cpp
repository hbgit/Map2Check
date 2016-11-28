#define DEBUG_TYPE "BlkLoopPass" 

#include <llvm/Pass.h>
#include <llvm/IR/Function.h> 
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Support/raw_ostream.h> 

#include <llvm/ADT/Statistic.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Analysis/LoopInfo.h>


#include <map>
#include <vector>
#include <iostream>

using namespace llvm;
using namespace std;


class BlkLoppPass : public FunctionPass {
	public:
		static	char ID;
		BlkLoppPass() : FunctionPass(ID) {}

		void getAnalysisUsage(AnalysisUsage &AU) const{
			AU.setPreservesAll();
			AU.addRequired<LoopInfo>();
		}
		
		void countBlocksInLoop(Loop *L, unsigned nesting){
			unsigned numBlocks = 0;
			Loop::block_iterator bb;
			
			for(bb = L->block_begin(); bb != L->block_end();++bb)
				numBlocks++;
			
			errs() << "Loop level " << nesting << " has " << numBlocks << " blocks\n";
			vector<Loop*> subLoops = L->getSubLoops();
			Loop::iterator j, f;
			
			for (j = subLoops.begin(), f = subLoops.end(); j != f; ++j)
				countBlocksInLoop(*j, nesting + 1);
		}
		
		virtual	bool runOnFunction(Function &F){
			LoopInfo &LI = getAnalysis<LoopInfo>();
			errs() << "Function " << F.getName() + "\n";
			for(LoopInfo::iterator i = LI.begin(), e = LI.end(); i != e; ++i)
				countBlocksInLoop(*i, 0);
			
			return(false);
		}

};


char BlkLoppPass::ID = 0;
static RegisterPass<BlkLoppPass> X("TestPass", "Test pass");
