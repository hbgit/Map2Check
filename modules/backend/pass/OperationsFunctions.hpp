#pragma once

#include <llvm/Pass.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>

class OperationsFunctions {

  Constant* OverflowAdd = NULL;
  Constant* OverflowAddUint = NULL;
  Constant* OverflowSub = NULL;
  Constant* OverflowSubUint = NULL;
    Constant* OverflowMul = NULL;
    Constant* OverflowSDiv = NULL;

public:
  Constant* getOverflowAdd() { return this->OverflowAdd; }
  Constant* getOverflowAddUint() { return this->OverflowAddUint; }
  Constant* getOverflowSub() { return this->OverflowSub; }
  Constant* getOverflowSubUint() { return this->OverflowSubUint; }
    Constant* getOverflowMul() { return this->OverflowMul; }
    Constant* getOverflowSDiv() { return this->OverflowSDiv; }
  
  OperationsFunctions(Function *F, LLVMContext* Ctx) {
    this->OverflowAdd = F->getParent()->
      getOrInsertFunction("map2check_binop_add",
			  Type::getVoidTy(*Ctx),
			  Type::getInt32Ty(*Ctx),
			  Type::getInt32Ty(*Ctx),
			  Type::getInt32Ty(*Ctx),
			  Type::getInt32Ty(*Ctx),
			  Type::getInt8PtrTy(*Ctx),
			  NULL);
			  
	this->OverflowAddUint = F->getParent()->
      getOrInsertFunction("map2check_binop_add_unit",
			  Type::getVoidTy(*Ctx),
			  Type::getInt32Ty(*Ctx),
			  Type::getInt32Ty(*Ctx),
			  Type::getInt32Ty(*Ctx),
			  Type::getInt32Ty(*Ctx),
			  Type::getInt8PtrTy(*Ctx),
			  NULL);

      this->OverflowSub = F->getParent()->
              getOrInsertFunction("map2check_binop_sub",
						  Type::getVoidTy(*Ctx),
						  Type::getInt32Ty(*Ctx),
						  Type::getInt32Ty(*Ctx),
						  Type::getInt32Ty(*Ctx),
						  Type::getInt32Ty(*Ctx),
						  Type::getInt8PtrTy(*Ctx),
						  NULL);
						  
	this->OverflowSubUint = F->getParent()->
              getOrInsertFunction("map2check_binop_sub_unit",
						  Type::getVoidTy(*Ctx),
						  Type::getInt32Ty(*Ctx),
						  Type::getInt32Ty(*Ctx),
						  Type::getInt32Ty(*Ctx),
						  Type::getInt32Ty(*Ctx),
						  Type::getInt8PtrTy(*Ctx),
						  NULL);

      this->OverflowMul = F->getParent()->
              getOrInsertFunction("map2check_binop_mul",
                                Type::getVoidTy(*Ctx),
                                Type::getInt32Ty(*Ctx),
                                Type::getInt32Ty(*Ctx),
                                Type::getInt32Ty(*Ctx),
                                Type::getInt32Ty(*Ctx),
                                Type::getInt8PtrTy(*Ctx),
                                NULL);

      this->OverflowSDiv = F->getParent()->
              getOrInsertFunction("map2check_binop_sdiv",
                                Type::getVoidTy(*Ctx),
                                Type::getInt32Ty(*Ctx),
                                Type::getInt32Ty(*Ctx),
                                Type::getInt32Ty(*Ctx),
                                Type::getInt32Ty(*Ctx),
                                Type::getInt8PtrTy(*Ctx),
                                NULL);

  }
};
