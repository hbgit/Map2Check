#include "NonDetPass.hpp"

namespace {
inline Instruction *BBIteratorToInst(BasicBlock::iterator i) {
  Instruction *pointer = reinterpret_cast<Instruction *>(&*i);
  return pointer;
}
}  // namespace

bool NonDetPass::runOnFunction(Function &F) {
  this->nonDetFunctions = make_unique<NonDetFunctions>(&F, &F.getContext());
  bool initializedFunctionName = false;
  for (Function::iterator bb = F.begin(), e = F.end(); bb != e; ++bb) {
    for (BasicBlock::iterator i = bb->begin(), e = bb->end(); i != e; ++i) {
      if (!initializedFunctionName) {
        IRBuilder<> builder(BBIteratorToInst(i));
        this->functionName = builder.CreateGlobalStringPtr(F.getName());
        initializedFunctionName = true;
      }
      if (CallInst *callInst = dyn_cast<CallInst>(&*i)) {
        currentInstruction = i;
        this->runOnCallInstruction(callInst, &F.getContext());
      }
    }
  }
  return true;
}

namespace {
#define CALL_IF_HELPER(type, nondet_type, c_type)                             \
  if ((calleeFunction->getName() == ("__VERIFIER_nondet_" #c_type))) {        \
    this->instrumentNonDet(NonDetType::nondet_type, calleeFunction);          \
    this->instrumentNonDet##type(callInst, Ctx);                              \
  } else if ((calleeFunction->getName() == ("map2check_non_det_" #c_type))) { \
    this->instrumentNonDet##type(callInst, Ctx);                              \
  }
}  // namespace

void NonDetPass::runOnCallInstruction(CallInst *callInst, LLVMContext *Ctx) {
  Function *calleeFunction = callInst->getCalledFunction();

  if (calleeFunction == NULL) {
    Value *v = callInst->getCalledValue();
    calleeFunction = dyn_cast<Function>(v->stripPointerCasts());
    if (calleeFunction == NULL) {
      return;
    }
  }

  // TODO(rafa.sa.xp@gmail.com): Should not rename __VERIFIER functions
  // clang-format off
  CALL_IF_HELPER(Integer, INTEGER, int)
  else CALL_IF_HELPER(Unsigned, UNSIGNED, unsigned)
  else CALL_IF_HELPER(Char, CHAR, char)
  else CALL_IF_HELPER(Pointer, POINTER, pointer)
  else CALL_IF_HELPER(Long, LONG, long)
  else CALL_IF_HELPER(Ushort, USHORT, ushort)
  else CALL_IF_HELPER(Ulong, ULONG, ulong)
  else CALL_IF_HELPER(Bool, BOOL, bool)
  else CALL_IF_HELPER(Uchar, UCHAR, uchar)
  else CALL_IF_HELPER(Pchar, PCHAR, pchar)
  else CALL_IF_HELPER(Size_t, SIZE_T, size_t)
  else CALL_IF_HELPER(Loff_t, LOFF_T, loff_t)
  else CALL_IF_HELPER(Sector_t, SECTOR_T, sector_t)
  else CALL_IF_HELPER(Uint, UINT, uint)
	 
  else if ((calleeFunction->getName() == "__VERIFIER_assume")) {
    this->instrumentNonDet(NonDetType::ASSUME, calleeFunction);
  }
  else if ((calleeFunction->getName() == "verifier.assume")) {
    this->instrumentNonDet(NonDetType::ASSUME, calleeFunction);
  }
  else if ((calleeFunction->getName() == "map2check_assume")) {
    // TODO(rafa.sa.xp@gmail.com): Implement method
  }
  // clang-format on
}

namespace {
#define INSTRUMENT_CASE_HELPER(nondet_type, c_type) \
  case (NonDetType::nondet_type): {                 \
    Twine non_det("map2check_non_det_" #c_type);    \
    calleeFunction->setName(non_det);               \
    break;                                          \
  }
}  // namespace
void NonDetPass::instrumentNonDet(NonDetType nonDetType,
                                  Function *calleeFunction) {
  switch (nonDetType) {
    INSTRUMENT_CASE_HELPER(INTEGER, int)
    INSTRUMENT_CASE_HELPER(UNSIGNED, unsigned)
    INSTRUMENT_CASE_HELPER(CHAR, char)
    INSTRUMENT_CASE_HELPER(POINTER, pointer)
    INSTRUMENT_CASE_HELPER(LONG, long)
    INSTRUMENT_CASE_HELPER(USHORT, ushort)
    INSTRUMENT_CASE_HELPER(ULONG, ulong)
    INSTRUMENT_CASE_HELPER(BOOL, bool)
    INSTRUMENT_CASE_HELPER(UCHAR, uchar)
    INSTRUMENT_CASE_HELPER(PCHAR, pchar)
    INSTRUMENT_CASE_HELPER(SIZE_T, size_t)
    INSTRUMENT_CASE_HELPER(LOFF_T, loff_t)
    INSTRUMENT_CASE_HELPER(SECTOR_T, sector_t)
    INSTRUMENT_CASE_HELPER(UINT, uint)
    case (NonDetType::ASSUME): {
      Twine assume("map2check_assume");
      calleeFunction->setName(assume);
      break;
    }
  }
}

namespace {
#define NONDET_IMPL_HELPER_POINTER(type)                                    \
  void NonDetPass::instrumentNonDet##type(CallInst *callInst,               \
                                          LLVMContext *Ctx) {               \
    auto j = this->currentInstruction;                                      \
    j++;                                                                    \
    IRBuilder<> builder(BBIteratorToInst(j));                               \
    Value *function_llvm = this->getFunctionNameValue();                    \
    Value *cast = builder.CreatePtrToInt(callInst, Type::getInt32Ty(*Ctx)); \
    DebugInfo debugInfo(Ctx, callInst);                                     \
    Value *args[] = {debugInfo.getLineNumberValue(),                        \
                     debugInfo.getScopeNumberValue(), cast, function_llvm}; \
    Constant *NonDetFunction =                                              \
        this->nonDetFunctions->getNonDet##type##Function();                 \
    builder.CreateCall(NonDetFunction, args);                               \
  }

#define NONDET_IMPL_HELPER_CAST(type)                                        \
  void NonDetPass::instrumentNonDet##type(CallInst *callInst,                \
                                          LLVMContext *Ctx) {                \
    auto j = this->currentInstruction;                                       \
    j++;                                                                     \
    IRBuilder<> builder(BBIteratorToInst(j));                                \
    Value *function_llvm = this->getFunctionNameValue();                     \
    Twine bitcast("map2check_nondet_cast_" #type);                           \
    Value *castInteger =                                                     \
        CastInst::CreateIntegerCast(callInst, Type::getInt32Ty(*Ctx), false, \
                                    bitcast, BBIteratorToInst(j));           \
    DebugInfo debugInfo(Ctx, callInst);                                      \
    Value *args[] = {debugInfo.getLineNumberValue(),                         \
                     debugInfo.getScopeNumberValue(), castInteger,           \
                     function_llvm};                                         \
    Constant *NonDetFunction =                                               \
        this->nonDetFunctions->getNonDet##type##Function();                  \
    builder.CreateCall(NonDetFunction, args);                                \
  }

#define NONDET_IMPL_HELPER(type)                                \
  void NonDetPass::instrumentNonDet##type(CallInst *callInst,   \
                                          LLVMContext *Ctx) {   \
    auto j = this->currentInstruction;                          \
    j++;                                                        \
    IRBuilder<> builder(BBIteratorToInst(j));                   \
    Value *function_llvm = this->getFunctionNameValue();        \
    DebugInfo debugInfo(Ctx, callInst);                         \
    Value *args[] = {debugInfo.getLineNumberValue(),            \
                     debugInfo.getScopeNumberValue(), callInst, \
                     function_llvm};                            \
    Constant *NonDetFunction =                                  \
        this->nonDetFunctions->getNonDet##type##Function();     \
    builder.CreateCall(NonDetFunction, args);                   \
  }
}  // namespace

NONDET_IMPL_HELPER(Integer)
NONDET_IMPL_HELPER(Unsigned)
NONDET_IMPL_HELPER(Uint)
NONDET_IMPL_HELPER_CAST(Char)
NONDET_IMPL_HELPER_CAST(Ushort)
NONDET_IMPL_HELPER_CAST(Long)
NONDET_IMPL_HELPER_CAST(Ulong)
NONDET_IMPL_HELPER_CAST(Bool)
NONDET_IMPL_HELPER_CAST(Uchar)
NONDET_IMPL_HELPER_CAST(Size_t)
NONDET_IMPL_HELPER_CAST(Loff_t)
NONDET_IMPL_HELPER_CAST(Sector_t)
NONDET_IMPL_HELPER_POINTER(Pchar)
NONDET_IMPL_HELPER_POINTER(Pointer)

char NonDetPass::ID = 1;
static RegisterPass<NonDetPass> X(
    "non_det", "Adds nondet calls for non deterministic methods (from SVCOMP)");
