#include "DebugInfo.hpp"

DebugInfo::DebugInfo(Context* ctx, Instruction* i) {
  DebugLoc location = i->getDebugLoc();

  if(location) {
    scope_number = location.getScope()->getMetadataID();
    line_number = location.getLine();

  }
  else {
    scope_number = 0;
    line_number  = 0;
  }

  this->scopeNumberValue = ConstantInt
    ::getSigned(Type::getInt32Ty(*this->Ctx), scope_number);
  this->lineNumberValue = ConstantInt
    ::getSigned(Type::getInt32Ty(*this->Ctx), line_number);
}

Value* DebugInfo::getScopeNumberValue() { return this->scopeNumberValue; }
Value* DebugInfo::getLineNumberValue() { return this->lineNumberValue; }
