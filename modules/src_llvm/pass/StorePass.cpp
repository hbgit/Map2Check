#define DEBUG_TYPE "FuncPass"
#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Type.h>
#include <iostream>

using namespace llvm;

struct StorePass : public FunctionPass {
        static char ID;
        StorePass() : FunctionPass(ID) {
        }

        virtual bool runOnFunction(Function &F) {
                LLVMContext& Ctx = F.getContext();

                Constant* map2check_add_store_pointer = F.getParent()->getOrInsertFunction(
                        "map2check_add_store_pointer",
                        Type::getVoidTy(Ctx),
                        Type::getInt8PtrTy(Ctx),
                        Type::getInt8PtrTy(Ctx),
                        NULL);
                /*
                 * Iterates over all instructions "i" of all functions and blocks
                 */
                for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
                        for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {

                          if (StoreInst* storeInst = dyn_cast<StoreInst>(&*i)) {

                            if(storeInst->getValueOperand()->getType()->isPointerTy()) {
                              errs() << "Var ponteiro\t"  << storeInst->getPointerOperand()->getName() << "\n";

                              auto j = i;
                              ++j;

                              //Adds map2check_free with destination address
                              //IRBuilder<> builder((Instruction*)i);
                              //auto var_address = storeInst->getPointerOperand();
                              //auto receives    = storeInst->getValueOperand();
                              //Value* args[] = {address};
                              //builder.CreateCall(add_store_pointer, args);
                            }

                            // errs() << "endereço da ponteiro" << storeInst->getValueOperand()->getName() << "\n";
                            // errs() << "endereço de destino"  << storeInst->getPointerOperand()->getName() << "\n";




                        }
                      }
                    }

        return true;
}
};


char StorePass::ID = 0;
static RegisterPass<StorePass> X("store_pass", "Injects code into certain functions");
