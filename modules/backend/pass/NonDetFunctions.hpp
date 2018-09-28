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

#define CONSTANT_GENERATOR(type)                                               \
private:                                                                       \
  Constant *NonDet##type = NULL;                                               \
                                                                               \
public:                                                                        \
  Constant *getNonDet##type##Function() { return this->NonDet##type; }

#define NON_DET_FUNCTIONS_HELPER(type, c_type)                                 \
  this->NonDet##type = F->getParent()->getOrInsertFunction(                    \
      "map2check_nondet_" #c_type, Type::getVoidTy(*Ctx),                      \
      Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx), Type::getInt32Ty(*Ctx),  \
      Type::getInt8PtrTy(*Ctx), NULL);

class NonDetFunctions {

  CONSTANT_GENERATOR(Integer)
  CONSTANT_GENERATOR(Unsigned)
  CONSTANT_GENERATOR(Char)
  CONSTANT_GENERATOR(Pointer)
  CONSTANT_GENERATOR(Long)
  CONSTANT_GENERATOR(Ushort)
  CONSTANT_GENERATOR(Ulong)
  CONSTANT_GENERATOR(Bool)
  CONSTANT_GENERATOR(Uchar)
  CONSTANT_GENERATOR(Pchar)
  CONSTANT_GENERATOR(Size_t)
  CONSTANT_GENERATOR(Loff_t)
  CONSTANT_GENERATOR(Sector_t)
  CONSTANT_GENERATOR(Uint)
  // Constant* NonDetAssume = NULL;

public:
  NonDetFunctions(Function *F, LLVMContext *Ctx) {
    NON_DET_FUNCTIONS_HELPER(Integer, int)
    NON_DET_FUNCTIONS_HELPER(Unsigned, unsigned)
    NON_DET_FUNCTIONS_HELPER(Pointer, pointer)
    NON_DET_FUNCTIONS_HELPER(Ushort, ushort)
    NON_DET_FUNCTIONS_HELPER(Char, char)
    NON_DET_FUNCTIONS_HELPER(Long, long)
    NON_DET_FUNCTIONS_HELPER(Ulong, ulong)
    NON_DET_FUNCTIONS_HELPER(Bool, bool)
    NON_DET_FUNCTIONS_HELPER(Uchar, uchar)
    NON_DET_FUNCTIONS_HELPER(Pchar, pchar)
    NON_DET_FUNCTIONS_HELPER(Size_t, size_t)
    NON_DET_FUNCTIONS_HELPER(Loff_t, loff_t)
    NON_DET_FUNCTIONS_HELPER(Sector_t, sector_t)
    NON_DET_FUNCTIONS_HELPER(Uint, uint)
  }
};
