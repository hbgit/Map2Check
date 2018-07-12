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
