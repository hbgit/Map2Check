#include "NonDetPass.hpp"

bool NonDetPass::runOnFunction(Function &F) {
  this->kleeFunctions =  make_unique<KleeFunctions>(&F, &F.getContext());
  bool initializedFunctionName = false; 
  for (Function::iterator bb = F.begin(), e = F.end();
       bb != e; ++bb) {
    for (BasicBlock::iterator i = bb->begin(),
         e = bb->end(); i != e; ++i) {

      if(!initializedFunctionName) {
    	  IRBuilder<> builder((Instruction*)&*i);
    	  this->functionName = builder
    			  .CreateGlobalStringPtr(F.getName());
    	  initializedFunctionName = true;
      }

      if (CallInst* callInst = dyn_cast<CallInst>(&*i)) {
    	  currentInstruction = i;
		  this->runOnCallInstruction(callInst, &F.getContext());
      }
    }      
  }
  return true;
}

void NonDetPass::runOnCallInstruction(CallInst *callInst, LLVMContext* Ctx) {
  Function* caleeFunction = callInst->getCalledFunction();

  if (caleeFunction == NULL) {
    Value* v = callInst->getCalledValue();
    caleeFunction = dyn_cast<Function>(v->stripPointerCasts());

    if(caleeFunction == NULL) {
      return;
    }
  }


  if ((caleeFunction->getName() == "__VERIFIER_nondet_int")) {
    this->instrumentKlee(NonDetType::INTEGER, caleeFunction);
    this->instrumentKleeInt(callInst, Ctx);
  }

  // TODO: FIX this hack
  else if ((caleeFunction->getName() == "map2check_non_det_int")) {
	  this->instrumentKleeInt(callInst, Ctx);
  }

  else if ((caleeFunction->getName() == "__VERIFIER_nondet_char")) {
    this->instrumentKlee(NonDetType::CHAR, caleeFunction);
    this->instrumentKleeChar(callInst, Ctx);
  }
  else if ((caleeFunction->getName() == "map2check_non_det_char")) {
    this->instrumentKleeChar(callInst, Ctx);
  }
  else if ((caleeFunction->getName() == "__VERIFIER_nondet_pointer")) {
    this->instrumentKlee(NonDetType::POINTER, caleeFunction);
    this->instrumentKleePointer(callInst, Ctx);
  }
  else if ((caleeFunction->getName() == "map2check_non_det_pointer")) {
    this->instrumentKleePointer(callInst, Ctx);
  }
  else if ((caleeFunction->getName() == "__VERIFIER_nondet_long")) {
    this->instrumentKlee(NonDetType::LONG, caleeFunction);
    this->instrumentKleeLong(callInst, Ctx);
  }
  else if ((caleeFunction->getName() == "map2check_non_det_long")) {
    this->instrumentKleeLong(callInst, Ctx);
  }
  else if ((caleeFunction->getName() == "__VERIFIER_assume")) {
    this->instrumentKlee(NonDetType::ASSUME, caleeFunction);
  }
  else if ((caleeFunction->getName() == "__VERIFIER_nondet_ushort")) {
    this->instrumentKlee(NonDetType::USHORT, caleeFunction);
   this->instrumentKleeUshort(callInst, Ctx);
  }
  else if ((caleeFunction->getName() == "map2check_non_det_ushort")) {
       this->instrumentKleeUshort(callInst, Ctx);
  }

  else if ((caleeFunction->getName() == "map2check_assume")) {
	  //TODO: Implement method
  }
  
}

void NonDetPass::instrumentKlee(NonDetType nonDetType, Function *caleeFunction) {
    switch(nonDetType) {
        case (NonDetType::INTEGER):
        {
            Twine non_det_int("map2check_non_det_int");
            caleeFunction->setName(non_det_int);
            break;
        }
         case (NonDetType::CHAR):{
             Twine non_det_char("map2check_non_det_char");
             caleeFunction->setName(non_det_char);
             break;
         }
          case (NonDetType::POINTER):{
             Twine non_det_pointer("map2check_non_det_pointer");
             caleeFunction->setName(non_det_pointer);
             break;
          }
          case (NonDetType::LONG):{
              Twine non_det_long("map2check_non_det_long");
             caleeFunction->setName(non_det_long);
             break;
         }
         case (NonDetType::USHORT):{
          Twine non_det_ushort("map2check_non_det_ushort");
          caleeFunction->setName(non_det_ushort);
          break;
         }
          case (NonDetType::ASSUME):{
             Twine assume("map2check_assume");
             caleeFunction->setName(assume);
             break;
         }
    }
}

void NonDetPass::instrumentKleeInt(CallInst* callInst, LLVMContext* Ctx) {
  auto j = this->currentInstruction;
  j++;
  IRBuilder<> builder((Instruction*)j);
  Value* function_llvm = this->getFunctionNameValue();
  DebugInfo debugInfo(Ctx, callInst);

  Value* args[] = {
    debugInfo.getLineNumberValue(),
    debugInfo.getScopeNumberValue(),
    callInst,
    function_llvm
     };


  Constant* KleeFunction = this->kleeFunctions->getKleeIntegerFunction();
  builder.CreateCall(KleeFunction, args);
}

 void NonDetPass::instrumentKleeChar(CallInst* callInst, LLVMContext* Ctx) {

   auto j = this->currentInstruction;
   j++;
   IRBuilder<> builder((Instruction*)j);
   Value* function_llvm = this->getFunctionNameValue();
   Twine bitcast("map2check_klee_char_cast");
   Value* charCast = CastInst::CreateIntegerCast(callInst, Type::getInt32Ty(*Ctx), false, bitcast, (Instruction*) j);
   DebugInfo debugInfo(Ctx, callInst);

   Value* args[] = {
	 debugInfo.getLineNumberValue(),
	 debugInfo.getScopeNumberValue(),
     // callInst,
     charCast,
     function_llvm
      };

   Constant* KleeFunction = this->kleeFunctions->getKleeCharFunction();
     builder.CreateCall(KleeFunction, args);
 }

 void NonDetPass::instrumentKleePointer(CallInst* callInst, LLVMContext* Ctx) {

   auto j = this->currentInstruction;
   j++;
   IRBuilder<> builder((Instruction*)j);
   Value* function_llvm = this->getFunctionNameValue();
   Twine bitcast("map2check_klee_pointer_cast");
   Value* charCast = CastInst::CreateIntegerCast(callInst, Type::getInt32Ty(*Ctx), false, bitcast, (Instruction*) j);
   DebugInfo debugInfo(Ctx, callInst);
  
   Value* args[] = {
		   debugInfo.getLineNumberValue(),
		   	 debugInfo.getScopeNumberValue(),
     // callInst,
     charCast,
     function_llvm
      };

   Constant* KleeFunction = this->kleeFunctions->getKleePointerFunction();
        builder.CreateCall(KleeFunction, args);
 }

 void NonDetPass::instrumentKleeUshort(CallInst* callInst, LLVMContext* Ctx) {

   auto j = this->currentInstruction;
   j++;
   IRBuilder<> builder((Instruction*)j);
   Value* function_llvm = this->getFunctionNameValue();
   Twine bitcast("map2check_klee_ushort_cast");
   Value* charCast = CastInst::CreateIntegerCast(callInst, Type::getInt32Ty(*Ctx), false, bitcast, (Instruction*) j);
   DebugInfo debugInfo(Ctx, callInst);
  
   Value* args[] = {
		   debugInfo.getLineNumberValue(),
		   		   	 debugInfo.getScopeNumberValue(),
     // callInst,
     charCast,
     function_llvm
      };

   Constant* KleeFunction = this->kleeFunctions->getKleeUshortFunction();
           builder.CreateCall(KleeFunction, args);
 }

 void NonDetPass::instrumentKleeLong(CallInst* callInst, LLVMContext* Ctx) {

   auto j = this->currentInstruction;
   j++;
   IRBuilder<> builder((Instruction*)j);
   Value* function_llvm = this->getFunctionNameValue();
   Twine bitcast("map2check_klee_long_cast");
   Value* charCast = CastInst::CreateIntegerCast(callInst, Type::getInt32Ty(*Ctx), false, bitcast, (Instruction*) j);
   DebugInfo debugInfo(Ctx, callInst);
   Value* args[] = {
		   debugInfo.getLineNumberValue(),
		   		   		   	 debugInfo.getScopeNumberValue(),
     // callInst,
     charCast,
     function_llvm
      };

   Constant* KleeFunction = this->kleeFunctions->getKleeLongFunction();
   builder.CreateCall(KleeFunction, args);
 }

char NonDetPass::ID = 1;
static RegisterPass<NonDetPass> X("non_det", "Adds klee calls for non deterministic methods (from SVCOMP)");


