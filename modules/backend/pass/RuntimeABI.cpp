/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#include "RuntimeABI.hpp"

#include <llvm/ADT/StringSwitch.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>

namespace Map2Check {

namespace {

llvm::FunctionType* fn(llvm::Type* ret, llvm::ArrayRef<llvm::Type*> params) {
  return llvm::FunctionType::get(ret, params, false);
}

}  // namespace

RuntimeCall runtimeCall(RuntimeHook hook, llvm::LLVMContext& ctx) {
  llvm::Type* v = llvm::Type::getVoidTy(ctx);
  llvm::Type* i16 = llvm::Type::getInt16Ty(ctx);
  llvm::Type* i32 = llvm::Type::getInt32Ty(ctx);
  llvm::Type* i64 = llvm::Type::getInt64Ty(ctx);
  llvm::Type* p = llvm::PointerType::get(ctx, 0);

  switch (hook) {
    // abi-mapping: map2check_init -> map2check_set_mode+map2check_init
    case RuntimeHook::SetMode:
      return {"map2check_set_mode", fn(v, {i32})};
    case RuntimeHook::Init:
      return {"map2check_init", fn(v, {})};
    // abi-mapping: map2check_success
    case RuntimeHook::Success:
      return {"map2check_success", fn(v, {})};
    // abi-mapping: map2check_track_bb (returns bbtrack_log_t*, ignored)
    case RuntimeHook::TrackBB:
      return {"map2check_save_basic_block_log", fn(p, {i32, p})};
    // abi-mapping: map2check_target_function
    case RuntimeHook::TargetReached:
      return {"map2check_reach_target", fn(v, {p, i32, i32})};
    // abi-mapping: map2check_assert (condition moves from first to last)
    case RuntimeHook::Assert:
      return {"map2check_is_valid_assert", fn(v, {i32, p, i32})};
    // abi-mapping: map2check_assume, map2check_assume_loop,
    // map2check_crab_assume (an i1 condition is zero-extended to i32)
    case RuntimeHook::Assume:
      return {"__VERIFIER_assume", fn(v, {i32})};
    // abi-mapping: map2check_binop_{add,sub,mul,sdiv,srem}; no scope argument
    case RuntimeHook::BinopAdd:
      return {"map2check_binop_add_signed", fn(v, {i64, i64, i32, i32, p})};
    case RuntimeHook::BinopSub:
      return {"map2check_binop_sub_signed", fn(v, {i64, i64, i32, i32, p})};
    case RuntimeHook::BinopMul:
      return {"map2check_binop_mul_signed", fn(v, {i64, i64, i32, i32, p})};
    case RuntimeHook::BinopSDiv:
      return {"map2check_binop_div_signed", fn(v, {i64, i64, i32, i32, p})};
    case RuntimeHook::BinopSRem:
      return {"map2check_binop_rem_signed", fn(v, {i64, i64, i32, i32, p})};
    // abi-mapping: map2check_malloc / map2check_calloc (sizes narrow to i32)
    case RuntimeHook::Malloc:
      return {"map2check_map_malloc", fn(v, {p, i32})};
    case RuntimeHook::Calloc:
      return {"map2check_map_calloc", fn(v, {p, i32, i32})};
    // abi-mapping: map2check_alloca / map2check_non_static_alloca
    case RuntimeHook::Alloca:
      return {"map2check_map_alloca", fn(v, {p, p, i32, i32, i32, i32})};
    case RuntimeHook::NonStaticAlloca:
      return {"map2check_map_non_static_alloca",
              fn(v, {p, p, i32, i32, i32, i32})};
    // abi-mapping: map2check_function
    case RuntimeHook::FunctionAddr:
      return {"map2check_map_funct_address", fn(v, {p, p})};
    // abi-mapping: map2check_add_store_pointer
    case RuntimeHook::StorePointer:
      return {"map2check_map_store_pointer", fn(v, {p, p, i32, p, i32, p})};
    // abi-mapping: map2check_load (now with line, scope, size, function)
    case RuntimeHook::Load:
      return {"map2check_check_load", fn(v, {p, i32, i32, i32, p})};
    // abi-mapping: map2check_free -> map2check_check_free+map2check_map_free
    // (name, pointer-variable slot, loaded value, scope, line, function)
    case RuntimeHook::CheckFree:
      return {"map2check_check_free", fn(v, {p, p, p, i32, i32, p})};
    case RuntimeHook::MapFree:
      return {"map2check_map_free", fn(v, {p, p, p, i32, i32, p})};
    // abi-mapping: map2check_free_resolved_address (i1 nullValid -> i16)
    case RuntimeHook::FreeResolved:
      return {"map2check_check_free_resolved_address", fn(v, {p, i32, p, i16})};
    // abi-mapping: map2check_posix
    case RuntimeHook::PosixMemalign:
      return {"map2check_map_posix_memalign", fn(v, {p, i32})};
    // abi-mapping: map2check_wasm_* (feature wasm of the library)
    case RuntimeHook::WasmMalloc:
      return {"map2check_wasm_malloc", fn(v, {i64, i64})};
    case RuntimeHook::WasmFree:
      return {"map2check_wasm_free", fn(v, {i64, i64})};
    case RuntimeHook::WasmCheckAccess:
      return {"map2check_wasm_check_access", fn(v, {i64, i64})};
  }
  return {nullptr, nullptr};
}

llvm::FunctionCallee getOrInsertRuntime(llvm::Module& module,
                                        RuntimeHook hook) {
  RuntimeCall call = runtimeCall(hook, module.getContext());
  return module.getOrInsertFunction(call.name, call.type);
}

// The nondet family of the mapping: map2check_non_det_<t> becomes
// __VERIFIER_nondet_<generator>, map2check_nondet_<t> becomes
// map2check_save_nondet_log_<t>.
std::string nonDetGenerator(llvm::StringRef cType) {
  llvm::StringRef suffix = llvm::StringSwitch<llvm::StringRef>(cType)
                               .Case("int", "int")
                               .Case("unsigned", "uint")
                               .Case("uint", "uint")
                               .Case("char", "char")
                               .Case("uchar", "uchar")
                               .Case("short", "short")
                               .Case("ushort", "ushort")
                               .Case("long", "long")
                               .Case("ulong", "ulong")
                               .Case("bool", "bool")
                               .Case("pointer", "pointer")
                               .Case("pchar", "pchar")
                               .Case("size_t", "size_t")
                               .Case("loff_t", "loff_t")
                               .Case("sector_t", "sector_t")
                               .Case("double", "double")
                               .Default("");
  return suffix.empty() ? "" : ("__VERIFIER_nondet_" + suffix).str();
}

std::string nonDetLogger(llvm::StringRef cType) {
  bool known = llvm::StringSwitch<bool>(cType)
                   .Cases("int", "unsigned", "uint", "char", "uchar", true)
                   .Cases("short", "ushort", "long", "ulong", "bool", true)
                   .Cases("pointer", "pchar", "size_t", "loff_t", true)
                   .Cases("sector_t", "double", true)
                   .Default(false);
  return known ? ("map2check_save_nondet_log_" + cType).str() : "";
}

llvm::FunctionType* nonDetLoggerType(llvm::LLVMContext& ctx) {
  llvm::Type* i32 = llvm::Type::getInt32Ty(ctx);
  llvm::Type* p = llvm::PointerType::get(ctx, 0);
  return fn(p, {i32, i32, i32, p, p});
}

NonDetValueSlot nonDetValueSlot(llvm::StringRef cType,
                                llvm::LLVMContext& ctx) {
  llvm::Type* i8 = llvm::Type::getInt8Ty(ctx);
  llvm::Type* i16 = llvm::Type::getInt16Ty(ctx);
  llvm::Type* i32 = llvm::Type::getInt32Ty(ctx);
  llvm::Type* i64 = llvm::Type::getInt64Ty(ctx);
  llvm::Type* p = llvm::PointerType::get(ctx, 0);
  llvm::Type* dbl = llvm::Type::getDoubleTy(ctx);
  // enum NONDET_TYPE: INTEGER 0, CHAR 1, POINTER 2, USHORT 3, LONG 4,
  // UNSIGNED 5, ULONG 6, BOOL 7, UCHAR 8, PCHAR 9, SIZE_T 10, LOFF_T 11,
  // SECTOR_T 12, UINT 13, SHORT 14, DOUBLE 15.
  return llvm::StringSwitch<NonDetValueSlot>(cType)
      .Case("int", {i32, true, 0})
      .Case("char", {i8, true, 1})
      .Case("pointer", {p, false, 2})
      .Case("ushort", {i16, false, 3})
      .Case("long", {i64, true, 4})
      .Case("unsigned", {i32, false, 5})
      .Case("ulong", {i64, false, 6})
      .Case("bool", {i32, false, 7})  // typedef int bool; _Bool widens
      .Case("uchar", {i8, false, 8})
      .Case("pchar", {p, false, 9})
      .Case("size_t", {i64, false, 10})
      .Case("loff_t", {i64, true, 11})  // long long
      .Case("sector_t", {i64, false, 12})  // unsigned long
      .Case("uint", {i32, false, 13})
      .Case("short", {i16, true, 14})
      .Case("double", {dbl, true, 15})
      .Default({nullptr, false, -1});
}

void allowUnwind(llvm::Function& fn) {
  fn.removeFnAttr(llvm::Attribute::NoUnwind);
  for (llvm::User* user : fn.users()) {
    auto* call = llvm::dyn_cast<llvm::CallBase>(user);
    if (call != nullptr && call->getCalledOperand() == &fn) {
      call->removeFnAttr(llvm::Attribute::NoUnwind);
    }
  }
}

const std::vector<std::string>& discardedProgramDefinitions() {
  static const std::vector<std::string> names = [] {
    std::vector<std::string> all = {"__VERIFIER_assume"};
    // The 16 generators the library exports (feature libfuzzer and klee).
    for (const char* type :
         {"bool", "char", "uchar", "short", "ushort", "int", "uint", "long",
          "ulong", "float", "double", "size_t", "loff_t", "sector_t",
          "pointer", "pchar"}) {
      all.push_back(std::string("__VERIFIER_nondet_") + type);
    }
    return all;
  }();
  return names;
}

const std::vector<std::string>& internalizedProgramDefinitions() {
  static const std::vector<std::string> names = {
      "__VERIFIER_assert",    "__VERIFIER_error",
      "get_current_step",     "get_next_step",
      "get_current_property", "set_false_result",
      "print_all_containers_as_json", "vcc_reset_meta_data"};
  return names;
}

bool resolveProgramDefinitions(llvm::Module& module) {
  bool changed = false;
  llvm::LLVMContext& ctx = module.getContext();
  const char* coreAssume = runtimeCall(RuntimeHook::Assume, ctx).name;

  for (const std::string& name : discardedProgramDefinitions()) {
    llvm::Function* program = module.getFunction(name);
    if (program == nullptr || program->isDeclaration()) continue;
    llvm::FunctionType* type = name == coreAssume
                                   ? runtimeCall(RuntimeHook::Assume, ctx).type
                                   : program->getFunctionType();
    program->setName("__map2check_program_" + name);
    llvm::FunctionCallee library = module.getOrInsertFunction(name, type);
    program->replaceAllUsesWith(library.getCallee());
    program->deleteBody();
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(ctx, "entry", program);
    new llvm::UnreachableInst(ctx, entry);
    program->setLinkage(llvm::GlobalValue::InternalLinkage);
    if (name == coreAssume) {
      allowUnwind(*llvm::cast<llvm::Function>(library.getCallee()));
    }
    changed = true;
  }

  for (const std::string& name : internalizedProgramDefinitions()) {
    llvm::Function* program = module.getFunction(name);
    if (program == nullptr || program->isDeclaration() ||
        program->hasLocalLinkage()) {
      continue;
    }
    program->setComdat(nullptr);
    program->setVisibility(llvm::GlobalValue::DefaultVisibility);
    program->setLinkage(llvm::GlobalValue::InternalLinkage);
    changed = true;
  }
  return changed;
}

}  // namespace Map2Check
