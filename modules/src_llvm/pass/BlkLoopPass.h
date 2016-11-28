// BlkLoopPass.h
#ifndef BLKLOOPPASS_H
#define BLKLOOPPASS_H

#include <string>
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


class BlkLoppPass : public FunctionPass {
	public:
		static	char ID;
		BlkLoppPass() : FunctionPass(ID) {}

		void getAnalysisUsage(AnalysisUsage &AU) const;
		void countBlocksInLoop(Loop *L, unsigned nesting);
		virtual	bool runOnFunction(Function &F);
};

#endif
