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

struct FuncPass : public FunctionPass {
        static char ID;
        FuncPass() : FunctionPass(ID) {
        }

        virtual bool runOnFunction(Function &F) {
                LLVMContext& Ctx = F.getContext();
                Constant* map2check_malloc = F.getParent()->getOrInsertFunction(
                        "map2check_malloc",
                        Type::getVoidTy(Ctx),
                        Type::getInt8PtrTy(Ctx),
                        Type::getInt64Ty(Ctx),
                        NULL);

                Constant* map2check_free = F.getParent()->getOrInsertFunction(
                        "map2check_free",
                        Type::getVoidTy(Ctx),
                        Type::getInt32PtrTy(Ctx),
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
                                        Function* f = callInst->getCalledFunction();

                                        // If 'f' equals NULL it could mean that a bitcast is taking place
                                        if(f == NULL) {
                                                Value* v = callInst->getCalledValue();
                                                f = dyn_cast<Function>(v->stripPointerCasts());

                                                if(f != NULL) {
                                                        if (f->getName() == "free") {
                                                          auto j = i;
                                                          ++j;

                                                          //Adds map2check_free with destination address
                                                          IRBuilder<> builder((Instruction*)i);
                                                          auto address = callInst->getArgOperand(0);
                                                          Value* args[] = {address};
                                                          builder.CreateCall(map2check_free, args);
                                                        }
                                                }

                                        }

                                        else {
                                                if (f->getName() == "malloc") {
                                                        auto j = i;
                                                        ++j;

                                                        //Adds map2check_malloc with allocated address and size
                                                        IRBuilder<> builder((Instruction*)j);
                                                        auto size = callInst->getArgOperand(0);
                                                        Value* args[] = {callInst, size};
                                                        builder.CreateCall(map2check_malloc, args);
                                                }
                                                else if (f->getName() == "non_det_int") {
                                                        Twine non_det("map2check_non_det_int");
                                                        f->setName(non_det);
                                                }
                                        }


                                }


                        }
                }

        return true;
}
};


char FuncPass::ID = 0;
static RegisterPass<FuncPass> X("func_pass", "Injects code into certain functions");
