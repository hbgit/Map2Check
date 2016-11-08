#define DEBUG_TYPE "inst"
#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Constants.h>


#include <llvm/Support/raw_ostream.h>
#include <iostream>

using namespace llvm;

namespace {
  struct Inst : public FunctionPass {
    static char ID;
    Inst() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
	  LLVMContext& Ctx = F.getContext();

      Constant* klee_malloc = F.getParent()
		->getOrInsertFunction(
							  "klee_malloc",
							  Type::getVoidTy(Ctx),
							  Type::getInt8PtrTy(Ctx),
							  Type::getInt64Ty(Ctx),
							  NULL);


      /*
	   * Iterates over all instructions "i" of all functions and blocks
	   */
      for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
		for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {

		  /*
		   * The next block check if the current instruction is a malloc or free,
		   * If you want to make support for other languages, this is where you
		   * add the equivalent functions
		   * Malloc: Adds the klee instrumentation to the pointer
		   * Free: TODO
		   */
		  if (CallInst* callInst = dyn_cast<CallInst>(&*i)) {
			auto f = callInst->getCalledFunction();
			if (f->getName() == "malloc") {
			  auto j = i;
			  ++j;

			  //Adds klee_malloc with allocated address and size
			  IRBuilder<> builder((Instruction*) j);
			  auto size = callInst->getArgOperand(0);
			  Value* args[] = {callInst, size};
			  builder.CreateCall(klee_malloc, args);
			}
			else if (f->getName() == "free") {
			  // Login for free
			}
			else if (f->getName() == "non_det_int") {
			  Twine non_det("klee_non_det_int");
			  f->setName(non_det);
			}

		  }
		}
	  }
	  return true;
	}
  };
}

char Inst::ID = 0;
static RegisterPass<Inst> X("functions_inst", "Injects code into certain functions", false, false);
