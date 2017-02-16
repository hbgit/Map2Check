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
    Constant* map2check_malloc = F
      .getParent()->getOrInsertFunction(
					"map2check_malloc",
					Type::getVoidTy(Ctx),
					Type::getInt8PtrTy(Ctx),
					Type::getInt64Ty(Ctx),
					NULL);

    Constant* map2check_free = F
      .getParent()->getOrInsertFunction(
					"map2check_free",
					Type::getVoidTy(Ctx),
					Type::getInt8PtrTy(Ctx),
					Type::getInt8PtrTy(Ctx),
					Type::getInt32Ty(Ctx),
					Type::getInt32Ty(Ctx),
					Type::getInt8PtrTy(Ctx),
					NULL);

    Constant* free_list_log = F
      .getParent()->getOrInsertFunction(
					"map2check_free_list_log",
					Type::getVoidTy(Ctx),
					NULL);
    /*
     * Iterates over all instructions "i" of all functions and blocks
     */
    for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
      for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {
	/*
	 * The next block check if the current instruction is a malloc
	 * or free,
	 * If you want to make support for other languages, this is 
	 * where you
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
		unsigned scope_number;
		unsigned line_number;

		DebugLoc location = callInst->getDebugLoc();
		if(location) {
		  scope_number = location.getScope()->getMetadataID();
		  line_number = location.getLine();
		}
		else {
		  scope_number = 0;
		  line_number  = 0;
		}

		ConstantInt* scope_value = ConstantInt
		  ::getSigned(Type::getInt32Ty(Ctx), scope_number);
		ConstantInt* line_value = ConstantInt
		  ::getSigned(Type::getInt32Ty(Ctx), line_number);

		LoadInst* li = dyn_cast<LoadInst>(callInst
						  ->getArgOperand(0));

		if(li) {
		  auto j = i;
		  ++j;

		  auto name = li->getPointerOperand()->getName();

		  IRBuilder<> builder((Instruction*)j);
		  Value* name_llvm = builder
		    .CreateGlobalStringPtr(name);

		  auto function_name = F.getName();
		  Value* function_llvm = builder
		  .CreateGlobalStringPtr(function_name);
		  
		  Twine non_det("bitcast_map2check");
		  Value* pointerCast = CastInst
		    ::CreatePointerCast(li->getPointerOperand(),
					Type::getInt8PtrTy(Ctx),
					non_det,
					(Instruction*) j);

		  Value* args[] = { name_llvm,
				    pointerCast,
				    scope_value,
				    line_value,
				    function_llvm
		  };

		  builder.CreateCall(map2check_free, args);
		}

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
	    else if (f->getName() == "__VERIFIER_nondet_int") {
	      Twine non_det("map2check_non_det_int");
	      f->setName(non_det);
	    }
	    else if (f->getName() == "free") {
	      Value* addr = callInst->getArgOperand(0)->stripPointerCasts();


	      unsigned scope_number;
	      unsigned line_number;

	      DebugLoc location = callInst->getDebugLoc();
	      if(location) {
		scope_number = location.getScope()->getMetadataID();
		line_number = location.getLine();
	      }
	      else {
		scope_number = 0;
		line_number  = 0;
	      }

	      ConstantInt* scope_value = ConstantInt
		::getSigned(Type::getInt32Ty(Ctx), scope_number);
	      ConstantInt* line_value = ConstantInt
		::getSigned(Type::getInt32Ty(Ctx), line_number);

	      LoadInst* li = dyn_cast<LoadInst>(callInst
						->getArgOperand(0)->stripPointerCasts());

	      if(li) {
		auto j = i;
		j;

		auto name = li->getPointerOperand()->getName();

		IRBuilder<> builder((Instruction*)j);
		Value* name_llvm = builder
		  .CreateGlobalStringPtr(name);
		Twine non_det("bitcast_map2check");

		 auto function_name = F.getName();
		  Value* function_llvm = builder
		  .CreateGlobalStringPtr(function_name);
		  
		Value* pointerCast = CastInst
		  ::CreatePointerCast(li->getPointerOperand(),
				      Type::getInt8PtrTy(Ctx),
				      non_det,
				      (Instruction*) j);

		Value* args[] = { name_llvm,
				  pointerCast,
				  scope_value,
				  line_value,
				  function_llvm
		};

		builder.CreateCall(map2check_free, args);
	      }

	    }
	  }


	}


      }
    }

    if(F.getName() == "main") {
      Function::iterator bb = F.end();
      bb--;

      BasicBlock::iterator i = bb->end();
      i--;

      IRBuilder<> builder((Instruction*)i);
      //Value* args[] = {};
      builder.CreateCall(free_list_log);
    }

    return true;
  }
};


char FuncPass::ID = 0;
static RegisterPass<FuncPass> X("func_pass", "Injects code into certain functions");
