#include "OverflowPass.h"

#include <llvm/IR/IntrinsicInst.h>

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
      Value *v = cI->getCalledValue();
      Function *calleeFunction = dyn_cast<Function>(v->stripPointerCasts());
      if (calleeFunction->getName() == "__VERIFIER_nondet_uint" ||
          calleeFunction->getName() == "map2check_non_det_uint") {
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
          if (F->getName().startswith("llvm.")) {
            const DbgDeclareInst *DDI = dyn_cast<DbgDeclareInst>(I);

            if (auto *N = dyn_cast<MDNode>(DDI->getVariable())) {
              // errs() << *N << "+++ \n";
              if (auto *DV = dyn_cast<DILocalVariable>(N)) {
                // errs() << *DV->getType() << "+++\n";
                if (auto *DT = dyn_cast<DIBasicType>(DV->getType())) {
                  if (DT->getName() == "unsigned int" ||
                      DT->getName() == "unsigned") {
                    // errs() << DT->getName() << "+++\n";
                    // errs() << DV->getName() << "+++\n";
                    this->listUnsignedVars.push_back(DV->getName());
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
    Value *v = callInst->getCalledValue();
    Function *calleeFunction = dyn_cast<Function>(v->stripPointerCasts());
    valueOp = calleeFunction->getName();

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

bool OverflowPass::runOnFunction(Function &F) {
  this->operationsFunctions =
      make_unique<OperationsFunctions>(&F, &F.getContext());
  Function::iterator functionIterator = F.begin();
  BasicBlock::iterator instructionIterator = functionIterator->begin();

  this->Ctx = &F.getContext();

  IRBuilder<> builder((Instruction *)&*instructionIterator);
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

        std::string operandName = firstOperand->getName();

        std::vector<std::string>::const_iterator iT;

        // checking for first operator
        // TODO: search by map2check_non_det_uint

        bool isUnsignedNonDet = false;
        iT = std::find(this->listUnsignedVars.begin(),
                       this->listUnsignedVars.end(), operandName);

        if (iT != this->listUnsignedVars.end() || isUnsignedNonDet) {
          // secondOperand->dump();
          this->valuesThatShouldBeUint.push_back(secondOperand);
        }
      }

      if (BinaryOperator *binOp = dyn_cast<BinaryOperator>(&*i)) {
        BasicBlock::iterator currentInstruction = i;

        Twine bitcast("map2check_pointer_cast");
        DebugInfo debugInfo(&F.getContext(), binOp);

        Constant *instrumentedFunction = NULL;

        Value *firstOperand = binOp->getOperand(0);
        Value *secondOperand = binOp->getOperand(1);
        currentInstruction++;
        IRBuilder<> builder(BBIteratorToInst(currentInstruction));

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
        // TODO: search by map2check_non_det_uint
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
        }

        // Check if is implicitly uint
        else if (std::find(this->valuesThatShouldBeUint.begin(),
                           this->valuesThatShouldBeUint.end(),
                           secondOperandValue) !=
                 this->valuesThatShouldBeUint.end()) {
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

        if (instrumentedFunction != NULL) {
          Value *firstOperand64Ty;
          if (firstOperand->getType() == Type::getInt32Ty(*Ctx)) {
            firstOperand64Ty = CastInst::CreateIntegerCast(
                firstOperand, Type::getInt64Ty(*Ctx), false, "cast",
                BBIteratorToInst(i));
          } else {
            firstOperand64Ty = firstOperand;
          }

          Value *secondOperand64Ty;
          if (secondOperand->getType() == Type::getInt32Ty(*Ctx)) {
            secondOperand64Ty = CastInst::CreateIntegerCast(
                secondOperand, Type::getInt64Ty(*Ctx), false, "cast",
                BBIteratorToInst(i));
          } else {
            secondOperand64Ty = secondOperand;
          }

          Value *args[] = {firstOperand64Ty, secondOperand64Ty,
                           debugInfo.getLineNumberValue(),
                           debugInfo.getScopeNumberValue(), functionName};
          builder.CreateCall(instrumentedFunction, args);

        } else {
          // errs() << binOp->getOpcodeName() <<" not supported\n";
        }
      }
    }
  }
  return true;
}

char OverflowPass::ID = 10;
static RegisterPass<OverflowPass> X(
    "check_overflow",
    "Validate overflow on signed integer dynamic operations tracking");
