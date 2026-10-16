/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

// The one place that names the runtime functions the passes emit.
//
// The runtime is map2check-library (Rust). Its core C API is the contract:
// every name and LLVM type below is a row of docs/cutover/abi-mapping.tsv of
// that repository, and the types are the core signatures of its map2check.h
// lowered for x86-64 (int32_t/uint32_t -> i32, int16_t -> i16,
// int64_t/uint64_t -> i64, pointers -> ptr). There is no compatibility layer:
// a pass builds its arguments in the core order and calls the core symbol.
// tests/unit/pass/RuntimeABITest.cpp checks this table against the mapping.

#ifndef MODULES_BACKEND_PASS_RUNTIMEABI_HPP_
#define MODULES_BACKEND_PASS_RUNTIMEABI_HPP_

#include <llvm/ADT/StringRef.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <string>
#include <vector>

namespace Map2Check {

enum class RuntimeHook {
  SetMode,          // map2check_set_mode(i32 mode)
  Init,             // map2check_init()
  Success,          // map2check_success()
  TrackBB,          // map2check_save_basic_block_log(i32 line, ptr fn) -> ptr
  TargetReached,    // map2check_reach_target(ptr fn, i32 scope, i32 line)
  Assert,           // map2check_is_valid_assert(i32 line, ptr fn, i32 cond)
  Assume,           // __VERIFIER_assume(i32 cond)
  BinopAdd,         // map2check_binop_add_signed(i64 a, i64 b, i32 bits,
  BinopSub,         //   i32 line, ptr fn), likewise sub/mul/div/rem
  BinopMul,
  BinopSDiv,
  BinopSRem,
  Malloc,           // map2check_map_malloc(ptr, i32 size)
  Calloc,           // map2check_map_calloc(ptr, i32 qty, i32 size)
  Alloca,           // map2check_map_alloca(ptr name, ptr addr, i32 size,
                    //   i32 size_primitive, i32 line, i32 scope)
  NonStaticAlloca,  // map2check_map_non_static_alloca(same as Alloca)
  FunctionAddr,     // map2check_map_funct_address(ptr name, ptr addr)
  StorePointer,     // map2check_map_store_pointer(ptr var, ptr value,
                    //   i32 scope, ptr name, i32 line, ptr fn)
  Load,             // map2check_check_load(ptr, i32 line, i32 scope,
                    //   i32 size, ptr fn)
  CheckFree,        // map2check_check_free(ptr name, ptr var_slot, ptr value,
                    //   i32 scope, i32 line, ptr fn)
  MapFree,          // map2check_map_free(same as CheckFree)
  FreeResolved,     // map2check_check_free_resolved_address(ptr, i32 line,
                    //   ptr fn, i16 is_null_valid)
  PosixMemalign,    // map2check_map_posix_memalign(ptr memptr, i32 size)
  WasmMalloc,       // map2check_wasm_malloc(i64 offset, i64 size)
  WasmFree,         // map2check_wasm_free(i64 offset, i64 line)
  WasmCheckAccess,  // map2check_wasm_check_access(i64 offset, i64 size)
  Last = WasmCheckAccess
};

/// A function of the core runtime API: name + type, for getOrInsertFunction.
struct RuntimeCall {
  const char* name;
  llvm::FunctionType* type;
};

RuntimeCall runtimeCall(RuntimeHook hook, llvm::LLVMContext& ctx);

/// Declares (or finds) the hook in `module`, with its core type.
llvm::FunctionCallee getOrInsertRuntime(llvm::Module& module, RuntimeHook hook);

/// __VERIFIER_nondet_<t> for the C type suffix used by the passes
/// ("int", "unsigned", "pointer", ...); "" when the type is not in the mapping.
std::string nonDetGenerator(llvm::StringRef cType);

/// map2check_save_nondet_log_<t>; "" when the type is not in the mapping.
std::string nonDetLogger(llvm::StringRef cType);

/// Type shared by every nondet logger:
/// (i32 line, i32 scope, i32 type_var, ptr value, ptr fn) -> ptr.
llvm::FunctionType* nonDetLoggerType(llvm::LLVMContext& ctx);

/// Metadata kind on the instructions a pass adds for the runtime's own use
/// (the nondet value slots), which are not program memory to track.
inline constexpr char kRuntimeMetadata[] = "map2check.runtime";

/// How a nondet value reaches map2check_save_nondet_log_<t>: the pass stores
/// it in an alloca of `type` (the C type of docs/api-deltas.md of the
/// library: bool widens to int, pointer/pchar are addresses) and passes
/// `typeVar`, the enum NONDET_TYPE number of the type
/// (Map2CheckTypes.h:41-58). `isSigned` says how an integer of another
/// width is converted to `type`.
struct NonDetValueSlot {
  llvm::Type* type;  // nullptr when the type is not in the mapping
  bool isSigned;
  int typeVar;
};
NonDetValueSlot nonDetValueSlot(llvm::StringRef cType, llvm::LLVMContext& ctx);

/// Removes `nounwind` from `fn` and from every call to it. An assume of the
/// library rejects an input by unwinding through its caller (D1 condition
/// 11), so a nounwind declaration or call site of it is undefined behaviour.
void allowUnwind(llvm::Function& fn);

/// Library exports a program may define itself (SV-COMP style); the link
/// would see two definitions. Two treatments, as develop had them:
///
/// - Discarded: the program's definition gives way to the library's. A
///   program-defined nondet generator (`int __VERIFIER_nondet_int(void)
///   { return 0; }`) would feed constants instead of the fuzzer's or KLEE's
///   input, and the program's __VERIFIER_assume would abort where the
///   library prunes the path. Every use is sent to a declaration of the same
///   name; the old body is renamed __map2check_program_<name>, made internal
///   and reduced to `unreachable` (passes later in the pipeline still visit
///   it, so it keeps a body).
/// - Internalized: the program keeps its own definition, with internal
///   linkage and the same name, so its calls use its body.
const std::vector<std::string>& discardedProgramDefinitions();
const std::vector<std::string>& internalizedProgramDefinitions();

/// Applies both lists to `module`. Idempotent; true when it changed
/// anything. The first pass of every pipeline (NonDetPass) calls it before
/// any instrumentation.
bool resolveProgramDefinitions(llvm::Module& module);

}  // namespace Map2Check

#endif  // MODULES_BACKEND_PASS_RUNTIMEABI_HPP_
