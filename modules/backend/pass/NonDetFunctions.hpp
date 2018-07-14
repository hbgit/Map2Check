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

using namespace llvm;

class NonDetFunctions {
  Constant* NonDetInteger = NULL;
  Constant* NonDetUnsigned = NULL;
  Constant* NonDetChar = NULL;
  Constant* NonDetPointer = NULL;
  Constant* NonDetLong = NULL;
  Constant* NonDetUshort = NULL;
  // Constant* NonDetAssume = NULL;

 public:
  Constant* getNonDetIntegerFunction() { return this->NonDetInteger; }
  Constant* getNonDetUnsignedFunction() { return this->NonDetUnsigned; }
  Constant* getNonDetCharFunction() { return this->NonDetChar; }
  Constant* getNonDetPointerFunction() { return this->NonDetPointer; }
  Constant* getNonDetLongFunction() { return this->NonDetLong; }
  Constant* getNonDetUshortFunction() { return this->NonDetUshort; }

  NonDetFunctions(Function* F, LLVMContext* Ctx) {
    this->NonDetInteger = F->getParent()->getOrInsertFunction(
        "map2check_nondet_int", Type::getVoidTy(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt8PtrTy(*Ctx));

    this->NonDetUnsigned = F->getParent()->getOrInsertFunction(
        "map2check_nondet_unsigned", Type::getVoidTy(*Ctx),
        Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt8PtrTy(*Ctx));

    this->NonDetPointer = F->getParent()->getOrInsertFunction(
        "map2check_nondet_pointer", Type::getVoidTy(*Ctx),
        Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt8PtrTy(*Ctx));

    this->NonDetUshort = F->getParent()->getOrInsertFunction(
        "map2check_nondet_ushort", Type::getVoidTy(*Ctx),
        Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt8PtrTy(*Ctx));

    this->NonDetChar = F->getParent()->getOrInsertFunction(
        "map2check_nondet_char", Type::getVoidTy(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt8PtrTy(*Ctx));

    this->NonDetLong = F->getParent()->getOrInsertFunction(
        "map2check_nondet_long", Type::getVoidTy(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),
        Type::getInt8PtrTy(*Ctx));
  }
};
