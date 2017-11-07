#include "DebugInfo.hpp"

DebugInfo::DebugInfo(LLVMContext* ctx, Instruction* i) {
  DebugLoc location = i->getDebugLoc();
  unsigned scope_number;
  unsigned line_number;

  if(location) {
    scope_number = location.getScope()->getMetadataID();
    line_number = location.getLine();

  }
  else {
    scope_number = 0;
    line_number  = 0;
  }
 
  /**
  if (MDNode *N = i->getMetadata("dbg")) {  // Here I is an LLVM instruction
        DIType Loc(*N);                      // DILocation is in DebugInfo.h        
        bool Name = Loc->isUnsignedDIType();
        
        //DILocation Loc(&N);                      // DILocation is in DebugInfo.h
        //unsigned Line = Loc.getLineNumber();
        
        errs() << Name << "------\n";
        //errs() << cast<MDString>(N->getOperand(0))->getString() << "\n";
        //errs() << cast<MDString>(*N) << "\n";
        
  }**/
  
  /**
  SmallVector<std::pair<unsigned, MDNode *>, 4> MDs;
	i->getAllMetadata(MDs);
	for (auto &MD : MDs) {
	  if (MDNode *N = MD.second) {
		  errs() << *N << "++++ \n";
		if (auto *subProgram = dyn_cast<DIBasicType>(N)) {
		  errs() << *subProgram << "+++++++++ \n";
		}
	  }
	}**/
  
  //errs() << "=================================== \n";
  
  /**if (MDNode* N = (*i).getMetadata("llvm.dbg.declare")) {
       errs() << cast<MDString>(N->getOperand(1))->getString() << "\n";
  }**/
  
  /**
  if(const DILocalVariable *DDI=dyn_cast<DILocalVariable>(i)) {
    errs() << &*DDI << "+++++++++++++ \n";
	//dbgInstFlag=true;
  }**/
    
  /**if(const DbgDeclareInst *DDI=dyn_cast<DbgDeclareInst>(i)) {
    errs() << *DDI << "+++++++++++++ \n";
	//dbgInstFlag=true;
  }**/

  this->scopeNumberValue = ConstantInt
    ::getSigned(Type::getInt32Ty(*ctx), scope_number);
  
  this->lineNumberValue = ConstantInt
    ::getSigned(Type::getInt32Ty(*ctx), line_number);

  this->lineNumInt = line_number;

}

Value* DebugInfo::getScopeNumberValue() { return this->scopeNumberValue; }

Value* DebugInfo::getLineNumberValue() { return this->lineNumberValue; }

unsigned DebugInfo::getLineNumberInt(){ return this->lineNumInt; }

StringRef DebugInfo::getVarName(){ return this->varName; }
