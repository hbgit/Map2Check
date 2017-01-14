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
                        Type::getInt32Ty(Ctx),
                        Type::getInt8PtrTy(Ctx),
                        Type::getInt32Ty(Ctx),
                        NULL);
                /*
                 * Iterates over all instructions "i" of all functions and blocks
                 */
                for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
                        for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {

                          if (StoreInst* storeInst = dyn_cast<StoreInst>(&*i)) {

                            if(storeInst->getValueOperand()->getType()->isPointerTy()) {

                              unsigned line_number;
                              unsigned scope_number;
                              DebugLoc location = storeInst->getDebugLoc();
                              if(location) {
                                scope_number = location.getScope()->getMetadataID();
                                line_number = location.getLine();
                              }
                              else {
                                scope_number = 0;
                                line_number  = 0;
                              }



                              ConstantInt* line_value = ConstantInt::getSigned(Type::getInt32Ty(Ctx), line_number);
                              ConstantInt* scope_value = ConstantInt::getSigned(Type::getInt32Ty(Ctx), scope_number);
                              Value* var_address = storeInst->getPointerOperand();
                              Value* receives    = storeInst->getValueOperand();

                              // errs() << "VAR: " << var_address->getName() << "\n";
                              // errs() << "SCOPE: " << scope_number << "\n";

                              auto j = i;

                              Twine bitcast("bitcast");

                      	      Value* varPointerCast = CastInst::CreatePointerCast(var_address, Type::getInt8PtrTy(Ctx), bitcast,(Instruction*) j);
                              Value* receivesPointerCast = CastInst::CreatePointerCast(receives, Type::getInt8PtrTy(Ctx), bitcast,(Instruction*) j);
                              ++j;

                              // Adds map2check_free with destination address
                              IRBuilder<> builder((Instruction*)j);
                              Value* name_llvm = builder.CreateGlobalStringPtr(var_address->getName());

                              Value* args[] = {varPointerCast, receivesPointerCast, scope_value, name_llvm, line_value};
                              builder.CreateCall(map2check_add_store_pointer, args);
                            }
                        }
                      }
                    }

        return true;
}
};


char StorePass::ID = 0;
static RegisterPass<StorePass> X("store_pass", "Injects code into certain functions");
