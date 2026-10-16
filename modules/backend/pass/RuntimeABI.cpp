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

}  // namespace Map2Check
