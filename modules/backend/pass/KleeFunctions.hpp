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

class KleeFunctions {

  Constant* KleeInteger = NULL;
  Constant* KleeChar = NULL;
  Constant* KleePointer = NULL;
  Constant* KleeLong = NULL;
  Constant* KleeUshort = NULL;
  Constant* KleeAssume = NULL;

public:
  Constant* getKleeIntegerFunction() { return this->KleeInteger; }
  Constant* getKleeCharFunction() { return this->KleeChar; }
  Constant* getKleePointerFunction() { return this->KleePointer; }
  Constant* getKleeLongFunction() { return this->KleeLong; }
  Constant* getKleeUshortFunction() { return this->KleeUshort; }
  
  KleeFunctions(Function &F, LLVMContext* Ctx) {    
    this->KleeInteger = F.getParent()->
      getOrInsertFunction("map2check_klee_int",
            Type::getVoidTy(*Ctx),
        Type::getInt32Ty(*Ctx),
        Type::getInt32Ty(*Ctx),
        Type::getInt32Ty(*Ctx),
            Type::getInt8PtrTy(*Ctx),
            NULL);

    this->KleePointer = F.getParent()->
      getOrInsertFunction("map2check_klee_pointer",
            Type::getVoidTy(*Ctx),
        Type::getInt32Ty(*Ctx),
        Type::getInt32Ty(*Ctx),
        Type::getInt32Ty(*Ctx),
            Type::getInt8PtrTy(*Ctx),
            NULL);

    this->KleeUshort = F.getParent()->
      getOrInsertFunction("map2check_klee_ushort",
            Type::getVoidTy(*Ctx),
        Type::getInt32Ty(*Ctx),
        Type::getInt32Ty(*Ctx),
        Type::getInt32Ty(*Ctx),
            Type::getInt8PtrTy(*Ctx),
            NULL);

  this->KleeChar = F.getParent()->
    getOrInsertFunction("map2check_klee_char",
			Type::getVoidTy(*Ctx),
      Type::getInt32Ty(*Ctx),
      Type::getInt32Ty(*Ctx),
      Type::getInt32Ty(*Ctx),
          Type::getInt8PtrTy(*Ctx),
          NULL);

    this->KleeLong = F.getParent()->
    getOrInsertFunction("map2check_klee_long",
			Type::getVoidTy(*Ctx),
      Type::getInt32Ty(*Ctx),
      Type::getInt32Ty(*Ctx),
      Type::getInt32Ty(*Ctx),
          Type::getInt8PtrTy(*Ctx),
          NULL);
  }
};
