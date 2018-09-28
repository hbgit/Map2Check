#pragma once

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

class LibraryFunctions {
  Constant* map2check_init = NULL;
  Constant* map2check_exit = NULL;
  Constant* map2check_track_bb = NULL;
  

 public:
  Constant* getInitFunction() { return this->map2check_init; }
  Constant* getTrackBBFunction() { return this->map2check_track_bb; }
  Constant* getExitFunction() { return this->map2check_exit; }  

  LibraryFunctions(Function* F, LLVMContext* Ctx) {
    this->map2check_init = F->getParent()->getOrInsertFunction(
							       "map2check_init", Type::getVoidTy(*Ctx), Type::getInt32Ty(*Ctx), NULL);

    this->map2check_exit = F->getParent()->getOrInsertFunction(
							       "map2check_success", Type::getVoidTy(*Ctx), NULL);

    this->map2check_exit = F->getParent()->getOrInsertFunction(
							       "map2check_success", Type::getVoidTy(*Ctx), NULL);

    this->map2check_track_bb = F->getParent()->getOrInsertFunction(
        "map2check_track_bb", Type::getVoidTy(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt8PtrTy(*Ctx), NULL);

    
  }
};
