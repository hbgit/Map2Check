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
      Constant* klee = F.getParent()
      ->getOrInsertFunction("klee",
                  Type::getVoidTy(Ctx),
                  Type::getInt32PtrTy(Ctx),
                  Type::getInt8PtrTy(Ctx),
                  NULL);


    /*
	 * Iterates over all instructions "i" of all functions and blocks
	 */
     for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
	   for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {

		 /*
		  * The next block check if the current instruction is a Allocation, if
		  * so, if the allocation is not a pointer, it instruments the "klee"
		  * function with its address and size
		  */
		 if (AllocaInst* allocInst = dyn_cast<AllocaInst>(&*i)) {
		  //  alloca_inst_pass(F, allocInst, i);

		   auto name = allocInst->getName();

			/*
			 * Check the name of the variable, if it is empty, it means that it
			 * is a temp var, and we do not need to care of it
			 */
		   if (name.compare("")){
			 auto type = allocInst->getAllocatedType();
			 // Checks if allocated type is not a pointer
			 if (!type->isPointerTy()) {
			   auto j = i;
			   ++j;

			   IRBuilder<> builder_alloc((Instruction*) j);
			   Value* name_llvm = builder_alloc.CreateGlobalStringPtr(name);

			   // Adds klee call with allocated address
			   IRBuilder<> builder((Instruction*) j);
			   Value* args[] = {allocInst, name_llvm };
			   builder.CreateCall(klee, args);
			 }
		   }
		 }
	   }
	   return true;
	 }
	}
};
}
	char Inst::ID = 0;
  static RegisterPass<Inst> X("alloca_inst", "Checks for allocations instructions", false, false);
