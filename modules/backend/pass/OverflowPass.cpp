/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#include "OverflowPass.hpp"

#include <memory>
#include <string>
#include <vector>

#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>

using llvm::CallInst;
using llvm::cast;
using llvm::CastInst;
using llvm::ConstantInt;
using llvm::DbgDeclareInst;
using llvm::DIBasicType;
using llvm::DILocalVariable;
using llvm::dyn_cast;
using llvm::IntegerType;
using llvm::IRBuilder;
using llvm::isa;
using llvm::LoadInst;
using std::make_unique;
using llvm::MDNode;
using llvm::StoreInst;
using llvm::Twine;
using llvm::FunctionCallee;

namespace {
inline Instruction *BBIteratorToInst(BasicBlock::iterator i) {
  Instruction *pointer = reinterpret_cast<Instruction *>(&*i);
  return pointer;
}
}  // namespace

void OverflowPass::hasNonDetUint(Instruction *I) {
  // I->dump();
  DebugInfo debugInfo(this->Ctx, I);
  // errs() << *debugInfo.getLineNumberValue() << "================\n";
}

void OverflowPass::listAllUintAssign(BasicBlock &B) {
  for (BasicBlock::iterator i = B.begin(), e = B.end(); i != e; ++i) {
    // i->dump();

    if (auto *cI = dyn_cast<CallInst>(&*i)) {
      Value *v = cI->getCalledOperand();
      Function *calleeFunction = dyn_cast<Function>(v->stripPointerCasts());
      if (calleeFunction &&
          (calleeFunction->getName() == "__VERIFIER_nondet_uint" ||
           calleeFunction->getName() == "map2check_non_det_uint")) {
        DebugInfo debugInfoCi(this->Ctx, cI);
        // errs() << debugInfoCi.getLineNumberInt() << "==================\n";

        // cI->dump();
        i++;
        i++;  // jump klee line
        // i->dump();
        if (StoreInst *storeI = dyn_cast<StoreInst>(&*i)) {
          // errs() << *storeI->getOperand(1) << "--\n";
          this->storeInstWithUint.push_back(&*storeI->getOperand(1));
        }
      }
    }
  }

  // Handle with the variable assignment v1 = i by a nondet UINT
  // Identify all store inst, then read each load inst from operand ZERO
  // to compare if the variable was identified as nondet UINT in
  // this->storeInstWithUint
  for (BasicBlock::iterator i = B.begin(), e = B.end(); i != e; ++i) {
    if (StoreInst *storeI = dyn_cast<StoreInst>(&*i)) {
      // Value* vst_1 = storeI->getOperand(1);

      if (LoadInst *loadI = dyn_cast<LoadInst>(&*storeI->getOperand(0))) {
        Value *vload = &*loadI->getPointerOperand();

        std::vector<Value *>::iterator iT;
        iT = std::find(this->storeInstWithUint.begin(),
                       this->storeInstWithUint.end(), vload);

        if (iT != this->storeInstWithUint.end()) {
          // DebugInfo debugInfoCi(this->Ctx, loadI);
          // errs() << debugInfoCi.getLineNumberInt() << ">>>>>>>>>>>> \n";
          // loadI->dump();
          // this->listLineNumUint.push_back(debugInfoCi.getLineNumberInt());
          this->storeInstWithUint.push_back(&*storeI->getOperand(1));
        }
      }
    }
  }

  // Read each load inst to identify if it has a nondet uint value
  for (BasicBlock::iterator i = B.begin(), e = B.end(); i != e; ++i) {
    if (LoadInst *loadI = dyn_cast<LoadInst>(&*i)) {
      // errs() << "-------------------- \n";
      // loadI->dump();
      DebugInfo debugInfoCi(this->Ctx, loadI);
      // errs() << debugInfoCi.getLineNumberInt() << "************* \n";

      Value *vload = &*loadI->getPointerOperand();

      std::vector<Value *>::iterator iT;
      iT = std::find(this->storeInstWithUint.begin(),
                     this->storeInstWithUint.end(), vload);

      if (iT != this->storeInstWithUint.end()) {
        DebugInfo debugInfoCi(this->Ctx, loadI);
        // errs() << debugInfoCi.getLineNumberInt() << "************* \n";
        this->listLineNumUint.push_back(debugInfoCi.getLineNumberInt());
      }
    }
  }
}

void OverflowPass::listAllUnsignedVar(Function &F) {
  for (Function::iterator BB = F.begin(), E = F.end(); BB != E; ++BB) {
    for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ++I) {
      // get the Metadata declared in the llvm intrinsic functions such as
      // llvm.dbg.declare()
      if (CallInst *CI = dyn_cast<CallInst>(I)) {
        if (Function *F = CI->getCalledFunction()) {
          if (F->getName().starts_with("llvm.")) {
            const DbgDeclareInst *DDI = dyn_cast<DbgDeclareInst>(I);
            if (!DDI) continue;

            if (auto *N = dyn_cast<MDNode>(DDI->getVariable())) {
              // errs() << *N << "+++ \n";
              if (auto *DV = dyn_cast<DILocalVariable>(N)) {
                // errs() << *DV->getType() << "+++\n";
                if (auto *DT = dyn_cast<DIBasicType>(DV->getType())) {
                  if (DT->getName() == "unsigned int" ||
                      DT->getName() == "unsigned") {
                    // errs() << DT->getName() << "+++\n";
                    // errs() << DV->getName() << "+++\n";
                    this->listUnsignedVars.push_back(DV->getName().str());
                    // errs() << DV->getLine() << "+++\n";
                    this->listLineNumUint.push_back(DV->getLine());
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

std::string OverflowPass::getValueNameOperator(Value *Vop) {
  std::string valueOp;
  std::ostringstream osstrtmp;

  if (isa<LoadInst>(Vop)) {
    LoadInst *LD100 = cast<LoadInst>(Vop);
    Value *C100 = LD100->getPointerOperand();
    valueOp = C100->getName().str();

  } else if (ConstantInt *CI = dyn_cast<ConstantInt>(Vop)) {
    if (CI->getBitWidth() <=
        32) {  // Of course, you can also change it to <= 64 if constIntValue is
               // a 64-bit integer, etc.
      osstrtmp << CI->getSExtValue();
      valueOp = osstrtmp.str();
    }
  } else if (CallInst *callInst = dyn_cast<CallInst>(Vop)) {
    Value *v = callInst->getCalledOperand();
    Function *calleeFunction = dyn_cast<Function>(v->stripPointerCasts());
    if (calleeFunction) {
      valueOp = calleeFunction->getName().str();
    }

  } else if (BinaryOperator *binOp = dyn_cast<BinaryOperator>(Vop)) {
    Value *fO1 = binOp->getOperand(0);
    if (isa<LoadInst>(fO1)) {
      LoadInst *Ld = cast<LoadInst>(fO1);
      Value *vOp = Ld->getPointerOperand();
      valueOp = vOp->getName().str();
    }
  }

  return valueOp;
}

PreservedAnalyses OverflowPass::run(Function &F,
                                    llvm::FunctionAnalysisManager &AM) {
  this->operationsFunctions =
      make_unique<OperationsFunctions>(&F, &F.getContext());
  Function::iterator functionIterator = F.begin();
  BasicBlock::iterator instructionIterator = functionIterator->begin();

  this->Ctx = &F.getContext();

  IRBuilder<> builder(reinterpret_cast<Instruction *>(&*instructionIterator));
  this->functionName = builder.CreateGlobalStringPtr(F.getName());

  this->listAllUnsignedVar(F);
  /**
    for(auto& B:F)
    {
    this->listAllUintAssign(B);
    }**/

  for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
    for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {
      if (this->errorLines.size() != 0) {
        Instruction *currentInstruction = BBIteratorToInst(i);
        DebugInfo debugInfo(&F.getContext(), currentInstruction);
        int line = debugInfo.getLineNumberInt();

        std::vector<int>::iterator it =
            std::find(this->errorLines.begin(), this->errorLines.end(), line);

        if (it != this->errorLines.end()) {
          IRBuilder<> builder(currentInstruction);
          Value *args[] = {debugInfo.getLineNumberValue(), functionName};
          builder.CreateCall(this->operationsFunctions->getOverflowError(),
                             args);

          this->errorLines.erase(it);
        }
      }

      if (StoreInst *storeInst = dyn_cast<StoreInst>(&*i)) {
        Value *firstOperand = storeInst->getValueOperand();
        Value *secondOperand = storeInst->getPointerOperand();

        std::string operandName = firstOperand->getName().str();

        std::vector<std::string>::const_iterator iT;

        // checking for first operator
        // TODO(hbgit): search by map2check_non_det_uint

        bool isUnsignedNonDet = false;
        iT = std::find(this->listUnsignedVars.begin(),
                       this->listUnsignedVars.end(), operandName);

        if (iT != this->listUnsignedVars.end() || isUnsignedNonDet) {
          // secondOperand->dump();
          this->valuesThatShouldBeUint.push_back(secondOperand);
        }
      }

      if (BinaryOperator *binOp = dyn_cast<BinaryOperator>(&*i)) {
        Twine bitcast("map2check_pointer_cast");
        DebugInfo debugInfo(&F.getContext(), binOp);

        FunctionCallee instrumentedFunction;

        Value *firstOperand = binOp->getOperand(0);
        Value *secondOperand = binOp->getOperand(1);
        // Insert the runtime check BEFORE the arithmetic operation: for
        // division by zero the operation itself traps (SIGFPE), so a check
        // inserted after it would never run.
        IRBuilder<> builder(BBIteratorToInst(i));

        // errs() << debugInfo.getLineNumberInt() << "=============\n";
        // get only variable names
        std::string lvaluep;
        std::string rvaluep;

        // get firstOperand
        lvaluep = getValueNameOperator(firstOperand);
        // errs() << lvaluep << "<<<< \n";

        // get secondOperand
        rvaluep = getValueNameOperator(secondOperand);
        // errs() << rvaluep << ">>>> \n";

        std::vector<std::string>::const_iterator iT;

        // checking for first operator
        // TODO(hbgit): search by map2check_non_det_uint
        bool isUnsigned = false;
        bool isUnsignedNonDet = false;
        iT = std::find(this->listUnsignedVars.begin(),
                       this->listUnsignedVars.end(), lvaluep);
        if (rvaluep == "map2check_non_det_uint") {
          isUnsignedNonDet = true;
        }

        if (iT != this->listUnsignedVars.end() || isUnsignedNonDet) {
          this->isUnitAssignment = true;
          isUnsigned = true;
        } else {
          this->isUnitAssignment = false;
          isUnsigned = false;
        }

        // checking for second operator
        if (!isUnsigned) {
          // errs() << "Checking second \n";
          iT = std::find(this->listUnsignedVars.begin(),
                         this->listUnsignedVars.end(), rvaluep);

          if (iT != this->listUnsignedVars.end() || isUnsignedNonDet) {
            this->isUnitAssignment = true;
          } else {
            this->isUnitAssignment = false;
          }
        }

        Value *firstOperandValue = firstOperand;
        if (isa<LoadInst>(firstOperand)) {
          LoadInst *Ld = cast<LoadInst>(firstOperand);
          firstOperandValue = Ld->getPointerOperand();
        }

        Value *secondOperandValue = secondOperand;
        if (isa<LoadInst>(secondOperand)) {
          LoadInst *Ld = cast<LoadInst>(secondOperand);
          secondOperandValue = Ld->getPointerOperand();
        }

        // Check if is implicitly uint
        if (std::find(this->valuesThatShouldBeUint.begin(),
                      this->valuesThatShouldBeUint.end(), firstOperandValue) !=
            this->valuesThatShouldBeUint.end()) {
          this->isUnitAssignment = true;
        } else if (std::find(this->valuesThatShouldBeUint.begin(),
                             this->valuesThatShouldBeUint.end(),
                             secondOperandValue) !=
                   this->valuesThatShouldBeUint.end()) {
          // Check if is implicitly uint
          this->isUnitAssignment = true;
        }

        this->isUnitAssignment = !binOp->hasNoSignedWrap();

        switch (binOp->getOpcode()) {
          case (Instruction::Add):
            if (this->isUnitAssignment) {
              // instrumentedFunction =
              // this->operationsFunctions->getOverflowAddUint();
            } else {
              instrumentedFunction =
                  this->operationsFunctions->getOverflowAdd();
            }
            break;
          case (Instruction::FAdd):

            break;
          case (Instruction::Sub):
            if (this->isUnitAssignment) {
              // instrumentedFunction =
              // this->operationsFunctions->getOverflowSubUint();
            } else {
              instrumentedFunction =
                  this->operationsFunctions->getOverflowSub();
            }
            break;
          case (Instruction::FSub):

            break;
          case (Instruction::Mul):
            if (this->isUnitAssignment) {
              // instrumentedFunction =
              // this->operationsFunctions->getOverflowMulUint();
            } else {
              instrumentedFunction =
                  this->operationsFunctions->getOverflowMul();
            }
            break;
          case (Instruction::FMul):

            break;
          case (Instruction::UDiv):

            break;
          case (Instruction::SDiv):
            instrumentedFunction = this->operationsFunctions->getOverflowSDiv();
            break;
          case (Instruction::FDiv):

            break;
          case (Instruction::URem):

            break;
          case (Instruction::SRem):
            instrumentedFunction = this->operationsFunctions->getOverflowSRem();
            break;
          case (Instruction::FRem):

            break;
          case (Instruction::Shl):

            break;
          case (Instruction::LShr):

            break;
          case (Instruction::AShr):

            break;
          case (Instruction::And):

            break;
          case (Instruction::Or):

            break;
          case (Instruction::Xor):

            break;
          case (Instruction::BinaryOpsEnd):

            break;
        }

        // SV-COMP defines no-overflow over "the resulting type of an
        // operation", so the check has to know that type's width: an i32 add
        // that wraps must be reported even though the same values never wrap
        // once widened. We therefore sign-extend both operands to the common
        // i64 ABI (lossless) and pass the operation's own width alongside, and
        // let the runtime range-check against it. Instrumenting only i32 would
        // silently miss every long/long long case (Juliet CWE190/CWE191).
        auto *opType = dyn_cast<IntegerType>(binOp->getType());
        if (instrumentedFunction && opType && opType->getBitWidth() <= 64 &&
            firstOperand->getType() == opType &&
            secondOperand->getType() == opType) {
          auto *I64 = Type::getInt64Ty(*Ctx);
          Value *firstOperand64Ty =
              builder.CreateSExtOrBitCast(firstOperand, I64, "m2c.sext");
          Value *secondOperand64Ty =
              builder.CreateSExtOrBitCast(secondOperand, I64, "m2c.sext");
          Value *width = ConstantInt::get(Type::getInt32Ty(*Ctx),
                                          opType->getBitWidth());

          Value *args[] = {firstOperand64Ty,
                           secondOperand64Ty,
                           width,
                           debugInfo.getLineNumberValue(),
                           debugInfo.getScopeNumberValue(),
                           functionName};
          builder.CreateCall(instrumentedFunction, args);

        } else {
          // errs() << binOp->getOpcodeName() <<" not supported\n";
        }
      }
    }
  }
  return PreservedAnalyses::none();
}

// --- New Pass Manager plugin registration ---
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "OverflowPass", LLVM_VERSION_STRING,
          [](llvm::PassBuilder& PB) {
            PB.registerPipelineParsingCallback(
                [](llvm::StringRef Name, llvm::FunctionPassManager& FPM,
                   llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "overflow-pass") {
                    FPM.addPass(OverflowPass());
                    return true;
                  }
                  return false;
                });
          }};
}
