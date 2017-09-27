#include "NonDetPass.hpp"

bool NonDetPass::runOnFunction(Function &F) {
  this->kleeFunctions =  make_unique<KleeFunctions>(F, &F.getContext());
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
	this->runOnCallInstruction(callInst);
      }
    }      
  }
  return true;
}

void NonDetPass::runOnCallInstruction(CallInst *callInst) {
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
    this->instrumentKleeInt(callInst);
  }
  // TODO: FIX this hack
  else if ((caleeFunction->getName() == "map2check_non_det_int")) {
      this->instrumentKleeInt(callInst);
  }
  else if ((caleeFunction->getName() == "__VERIFIER_nondet_char")) {
    this->instrumentKlee(NonDetType::CHAR, caleeFunction);
    this->instrumentKleeChar(callInst);
  }
  else if ((caleeFunction->getName() == "map2check_non_det_char")) {
    this->instrumentKleeChar(callInst);
  }
  else if ((caleeFunction->getName() == "__VERIFIER_nondet_pointer")) {
    this->instrumentKlee(NonDetType::POINTER, caleeFunction);
    this->instrumentKleePointer(callInst);
  }
  else if ((caleeFunction->getName() == "map2check_non_det_pointer")) {
    this->instrumentKleePointer(callInst);
  }
  else if ((caleeFunction->getName() == "__VERIFIER_nondet_long")) {
    this->instrumentKlee(NonDetType::LONG, caleeFunction);
    this->instrumentKleeLong(callInst);
  }
  else if ((caleeFunction->getName() == "map2check_non_det_long")) {
    this->instrumentKleeLong(callInst);
  }
  else if ((caleeFunction->getName() == "__VERIFIER_assume")) {
    this->instrumentKlee(NonDetType::ASSUME, caleeFunction);
  }
  else if ((caleeFunction->getName() == "__VERIFIER_nondet_ushort")) {
    this->instrumentKlee(NonDetType::USHORT, caleeFunction);
   this->instrumentKleeUshort(callInst);
  }
  else if ((caleeFunction->getName() == "map2check_non_det_ushort")) {
       this->instrumentKleeUshort(callInst);
  }

  else if ((caleeFunction->getName() == "map2check_assume")) {
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
        // case (NonDetType::CHAR):{
        //     Twine non_det_char("map2check_non_det_char");
        //     caleeFunction->setName(non_det_char);
        //     break;
        // }
        //  case (NonDetType::POINTER):{
        //     Twine non_det_pointer("map2check_non_det_pointer");
        //     caleeFunction->setName(non_det_pointer);
        //     break;
        //  }
        //  case (NonDetType::LONG):{
        //      Twine non_det_long("map2check_non_det_long");
        //     caleeFunction->setName(non_det_long);
        //     break;
        // }
        // case (NonDetType::USHORT):{
        //  Twine non_det_ushort("map2check_non_det_ushort");
        //  caleeFunction->setName(non_det_ushort);
        //  break;
        // }
        //  case (NonDetType::ASSUME):{
        //     Twine assume("map2check_assume");
        //     caleeFunction->setName(assume);
        //     break;
        // }
    }
}

void NonDetPass::instrumentKleeInt(CallInst* callInst, LLVMContext* Ctx){

  CallInst* callInst = dyn_cast<CallInst>(&*this->currentInstruction);
  
  auto j = (Instruction*) callInst;
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

  builder.CreateCall(this->kleeFunctions->getKleeIntegerFunction(), args);
}


// void MemoryTrackPass::instrumentKlee(NonDetType nonDetType) {
//     switch(nonDetType) {
//         case (NonDetType::INTEGER):
//         {
//             Twine non_det_int("map2check_non_det_int");
//             this->caleeFunction->setName(non_det_int);
//             break;
//         }
//         case (NonDetType::CHAR):{
//             Twine non_det_char("map2check_non_det_char");
//             this->caleeFunction->setName(non_det_char);
//             break;
//         }
//          case (NonDetType::POINTER):{
//             Twine non_det_pointer("map2check_non_det_pointer");
//             this->caleeFunction->setName(non_det_pointer);
//             break;
//          }
//          case (NonDetType::LONG):{
//              Twine non_det_long("map2check_non_det_long");
//             this->caleeFunction->setName(non_det_long);
//             break;
//         }
//         case (NonDetType::USHORT):{
//          Twine non_det_ushort("map2check_non_det_ushort");
//          this->caleeFunction->setName(non_det_ushort);
//          break;
//         }
//          case (NonDetType::ASSUME):{
//             Twine assume("map2check_assume");
//             this->caleeFunction->setName(assume);
//             break;
//         }
//     }




// }

// void MemoryTrackPass::instrumentKleeInt() {

//   CallInst* callInst = dyn_cast<CallInst>(&*this->currentInstruction);
//   auto j = this->currentInstruction;
//   j++;
//   IRBuilder<> builder((Instruction*)j);
//   Value* function_llvm = builder.CreateGlobalStringPtr
//       (currentFunction->getName());

//   Value* args[] = {
//     this->line_value,
//     this->scope_value,
//     callInst,
//     function_llvm
//      };

//   builder.CreateCall(this->map2check_klee_int, args);
// }

// void MemoryTrackPass::instrumentKleeChar() {

//   CallInst* callInst = dyn_cast<CallInst>(&*this->currentInstruction);
//   auto j = this->currentInstruction;
//   j++;
//   IRBuilder<> builder((Instruction*)j);
//   Value* function_llvm = builder.CreateGlobalStringPtr
//       (currentFunction->getName());
//   Twine bitcast("map2check_klee_char_cast");
//   Value* charCast = CastInst::CreateIntegerCast(callInst, Type::getInt32Ty(*this->Ctx), false, bitcast, (Instruction*) j);
  
//   Value* args[] = {
//     this->line_value,
//     this->scope_value,
//     // callInst,
//     charCast,
//     function_llvm
//      };

//   builder.CreateCall(this->map2check_klee_char, args);
// }

// void MemoryTrackPass::instrumentKleePointer() {

//   CallInst* callInst = dyn_cast<CallInst>(&*this->currentInstruction);
//   auto j = this->currentInstruction;
//   j++;
//   IRBuilder<> builder((Instruction*)j);
//   Value* function_llvm = builder.CreateGlobalStringPtr
//       (currentFunction->getName());
//   Twine bitcast("map2check_klee_pointer_cast");
//   Value* charCast = CastInst::CreateIntegerCast(callInst, Type::getInt32Ty(*this->Ctx), false, bitcast, (Instruction*) j);
  
//   Value* args[] = {
//     this->line_value,
//     this->scope_value,
//     // callInst,
//     charCast,
//     function_llvm
//      };

//   builder.CreateCall(this->map2check_klee_pointer, args);
// }

// void MemoryTrackPass::instrumentKleeUshort() {

//   CallInst* callInst = dyn_cast<CallInst>(&*this->currentInstruction);
//   auto j = this->currentInstruction;
//   j++;
//   IRBuilder<> builder((Instruction*)j);
//   Value* function_llvm = builder.CreateGlobalStringPtr
//       (currentFunction->getName());
//   Twine bitcast("map2check_klee_ushort_cast");
//   Value* charCast = CastInst::CreateIntegerCast(callInst, Type::getInt32Ty(*this->Ctx), false, bitcast, (Instruction*) j);
  
//   Value* args[] = {
//     this->line_value,
//     this->scope_value,
//     // callInst,
//     charCast,
//     function_llvm
//      };

//   builder.CreateCall(this->map2check_klee_ushort, args);
// }

// void MemoryTrackPass::instrumentKleeLong() {

//   CallInst* callInst = dyn_cast<CallInst>(&*this->currentInstruction);
//   auto j = this->currentInstruction;
//   j++;
//   IRBuilder<> builder((Instruction*)j);
//   Value* function_llvm = builder.CreateGlobalStringPtr
//       (currentFunction->getName());
//   Twine bitcast("map2check_klee_long_cast");
//   Value* charCast = CastInst::CreateIntegerCast(callInst, Type::getInt32Ty(*this->Ctx), false, bitcast, (Instruction*) j);
  
//   Value* args[] = {
//     this->line_value,
//     this->scope_value,
//     // callInst,
//     charCast,
//     function_llvm
//      };

//   builder.CreateCall(this->map2check_klee_ushort, args);
// }
