#define DEBUG_TYPE "AssertsPass"
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
// #include "AssertsPass.h"

using namespace llvm;

struct AssertsPass : public FunctionPass {
private:
  std::string target_function;
public:

        static char ID;
        AssertsPass() : FunctionPass(ID) {
          this->target_function = "_______MAP2CHECK___DEFAULT____TARGET_____";
        }

        AssertsPass(std::string functions) : FunctionPass(ID) {
          this->target_function = functions;
        }

        virtual bool runOnFunction(Function &F) {
                LLVMContext& Ctx = F.getContext();

                Constant* map2check_target_function = F.getParent()->getOrInsertFunction(
                        "map2check_target_function",
                        Type::getVoidTy(Ctx),
			Type::getInt8PtrTy(Ctx),
			Type::getInt32Ty(Ctx),
                        Type::getInt32Ty(Ctx),
                        NULL);
                /*
                 * Iterates over all instructions "i" of all functions and blocks
                 */

		auto name = F.getName();
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
					
                                        if(f == NULL) {
                                                Value* v = callInst->getCalledValue();

                                                f = dyn_cast<Function>(v->stripPointerCasts());

                                                if(f != NULL) {

                                                        if ( !this->target_function.compare(f->getName())) {
                                                          auto j = i;

                                                          IRBuilder<> builder((Instruction*)j);
                                                          builder.CreateCall(map2check_target_function);

                                                        }
                                                }

                                        }

                                        else {

					  
					  if (!this->target_function.compare(f->getName())) {
					    auto j = i;
					    
					    IRBuilder<> builder((Instruction*)j);
					    
					    unsigned line_number;
					    unsigned scope_number;

					    DebugLoc location = callInst->getDebugLoc();
					    if(location) {
					      scope_number = location.getScope()->getMetadataID();
					      line_number = location.getLine();
					    }
					    else {
					      scope_number = 0;
					      line_number  = 0;
					    }

					    ConstantInt* scope_value = ConstantInt::getSigned(Type::getInt32Ty(Ctx), scope_number);
					    ConstantInt* line_value = ConstantInt::getSigned(Type::getInt32Ty(Ctx), line_number);

					    Value* name_llvm = builder.CreateGlobalStringPtr(this->target_function);
					    
					    Value* args[] = {name_llvm, scope_value, line_value };
					    builder.CreateCall(map2check_target_function, args);
					  }
                                        }



                                }


                        }
                }


        return true;
}
};


char AssertsPass::ID = 0;
static RegisterPass<AssertsPass> X("asserts_pass", "Injects code into certain functions");
