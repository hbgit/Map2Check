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

  bool dbgInstFlag=false;
  MDNode * MD; 
  const Value* val;


  if(const DbgValueInst *DVI=dyn_cast<DbgValueInst>(i)) {
    val=DVI->getValue();
	//MD = DVI->getVariable();
    errs() << DVI->getVariable() << "+++++++++++++ \n";
	dbgInstFlag=true;
  } else if(const DbgDeclareInst *DDI=dyn_cast<DbgDeclareInst>(i)) {
	//val=DDI->getAddress();
	//MD = 
    errs() << DDI->getVariable() << "+++++++++++++ \n";
	dbgInstFlag=true;
  }

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
