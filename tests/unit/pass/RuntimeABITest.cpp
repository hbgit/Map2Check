/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

// The hook table of the passes against docs/cutover/abi-mapping.tsv: every
// emitted name is a core symbol of map2check-library, none is a name of the
// C runtime, and the LLVM types are the core signatures of map2check.h.

#include <gtest/gtest.h>

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "../../../modules/backend/pass/RuntimeABI.hpp"
#include "AbiMapping.hpp"

using Map2Check::nonDetGenerator;
using Map2Check::nonDetLogger;
using Map2Check::RuntimeCall;
using Map2Check::runtimeCall;
using Map2Check::RuntimeHook;

namespace {

std::vector<RuntimeHook> allHooks() {
  std::vector<RuntimeHook> hooks;
  for (int h = 0; h <= static_cast<int>(RuntimeHook::Last); ++h) {
    hooks.push_back(static_cast<RuntimeHook>(h));
  }
  return hooks;
}

// The C types of the nondet family in the mapping (map2check_non_det_<t>).
const std::vector<std::string> kNonDetTypes = {
    "int",   "unsigned", "char",   "pointer", "long",   "ushort",
    "short", "ulong",    "bool",   "uchar",   "pchar",  "size_t",
    "loff_t", "sector_t", "uint",  "double", "float",
    "longlong", "ulonglong", "u8", "u16", "u32"};

}  // namespace

TEST(RuntimeABI, NamesMatchMapping) {
  llvm::LLVMContext ctx;
  std::set<std::string> core = m2c_test::loadCoreSymbols(MAP2CHECK_ABI_MAPPING_TSV);
  ASSERT_FALSE(core.empty());
  for (RuntimeHook h : allHooks()) {
    RuntimeCall call = runtimeCall(h, ctx);
    ASSERT_NE(call.name, nullptr) << "hook " << static_cast<int>(h);
    ASSERT_NE(call.type, nullptr) << call.name;
    EXPECT_TRUE(core.count(call.name)) << call.name;
  }
}

TEST(RuntimeABI, NoLegacyNameRemains) {
  llvm::LLVMContext ctx;
  std::set<std::string> legacy =
      m2c_test::loadLegacyOnlySymbols(MAP2CHECK_ABI_MAPPING_TSV);
  ASSERT_TRUE(legacy.count("map2check_track_bb"));  // the loader sees the C names
  for (RuntimeHook h : allHooks()) {
    EXPECT_FALSE(legacy.count(runtimeCall(h, ctx).name))
        << runtimeCall(h, ctx).name;
  }
  for (const std::string& t : kNonDetTypes) {
    EXPECT_FALSE(legacy.count(nonDetGenerator(t))) << t;
    EXPECT_FALSE(legacy.count(nonDetLogger(t))) << t;
  }
}

TEST(RuntimeABI, HookNamesAreDistinct) {
  llvm::LLVMContext ctx;
  std::set<std::string> seen;
  for (RuntimeHook h : allHooks()) {
    EXPECT_TRUE(seen.insert(runtimeCall(h, ctx).name).second)
        << runtimeCall(h, ctx).name;
  }
}

TEST(RuntimeABI, NonDetNamesMatchMapping) {
  std::set<std::string> core = m2c_test::loadCoreSymbols(MAP2CHECK_ABI_MAPPING_TSV);
  for (const std::string& t : kNonDetTypes) {
    EXPECT_TRUE(core.count(nonDetGenerator(t))) << nonDetGenerator(t);
    EXPECT_TRUE(core.count(nonDetLogger(t))) << nonDetLogger(t);
  }
  // The mapping renames the "unsigned" generator to the uint one, but keeps a
  // dedicated "unsigned" logger.
  EXPECT_EQ(nonDetGenerator("unsigned"), "__VERIFIER_nondet_uint");
  EXPECT_EQ(nonDetLogger("unsigned"), "map2check_save_nondet_log_unsigned");
  EXPECT_EQ(nonDetGenerator("not_a_type"), "");
  EXPECT_EQ(nonDetLogger("not_a_type"), "");
}

// The mapping rows the generators replace: map2check_non_det_<t> and
// map2check_nondet_<t> resolve to exactly these core names.
TEST(RuntimeABI, NonDetNamesFollowTheirRows) {
  for (const m2c_test::AbiRow& row :
       m2c_test::loadAbiMapping(MAP2CHECK_ABI_MAPPING_TSV)) {
    const std::string generatorPrefix = "map2check_non_det_";
    const std::string loggerPrefix = "map2check_nondet_";
    if (row.cSymbol.rfind(generatorPrefix, 0) == 0) {
      ASSERT_EQ(row.coreSymbols.size(), 1u) << row.cSymbol;
      EXPECT_EQ(nonDetGenerator(row.cSymbol.substr(generatorPrefix.size())),
                row.coreSymbols[0]);
    } else if (row.cSymbol.rfind(loggerPrefix, 0) == 0) {
      ASSERT_EQ(row.coreSymbols.size(), 1u) << row.cSymbol;
      EXPECT_EQ(nonDetLogger(row.cSymbol.substr(loggerPrefix.size())),
                row.coreSymbols[0]);
    }
  }
}

// The LLVM types are the core C signatures of map2check.h, lowered for
// x86-64: int32_t/uint32_t -> i32, int16_t -> i16, int64_t/uint64_t -> i64,
// pointers -> ptr. A pointer return (save_basic_block_log) stays ptr.
TEST(RuntimeABI, TypesMatchCoreSignatures) {
  llvm::LLVMContext ctx;
  llvm::Type* v = llvm::Type::getVoidTy(ctx);
  llvm::Type* i16 = llvm::Type::getInt16Ty(ctx);
  llvm::Type* i32 = llvm::Type::getInt32Ty(ctx);
  llvm::Type* i64 = llvm::Type::getInt64Ty(ctx);
  llvm::Type* p = llvm::PointerType::get(ctx, 0);
  auto fn = [](llvm::Type* ret, std::vector<llvm::Type*> params) {
    return llvm::FunctionType::get(ret, params, false);
  };
  struct Expected {
    RuntimeHook hook;
    const char* name;
    llvm::FunctionType* type;
  };
  const std::vector<Expected> expected = {
      {RuntimeHook::SetMode, "map2check_set_mode", fn(v, {i32})},
      {RuntimeHook::Init, "map2check_init", fn(v, {})},
      {RuntimeHook::Success, "map2check_success", fn(v, {})},
      {RuntimeHook::TrackBB, "map2check_save_basic_block_log", fn(p, {i32, p})},
      {RuntimeHook::TargetReached, "map2check_reach_target", fn(v, {p, i32, i32})},
      {RuntimeHook::Assert, "map2check_is_valid_assert", fn(v, {i32, p, i32})},
      {RuntimeHook::Assume, "__VERIFIER_assume", fn(v, {i32})},
      {RuntimeHook::BinopAdd, "map2check_binop_add_signed", fn(v, {i64, i64, i32, i32, p})},
      {RuntimeHook::BinopSub, "map2check_binop_sub_signed", fn(v, {i64, i64, i32, i32, p})},
      {RuntimeHook::BinopMul, "map2check_binop_mul_signed", fn(v, {i64, i64, i32, i32, p})},
      {RuntimeHook::BinopSDiv, "map2check_binop_div_signed", fn(v, {i64, i64, i32, i32, p})},
      {RuntimeHook::BinopSRem, "map2check_binop_rem_signed", fn(v, {i64, i64, i32, i32, p})},
      {RuntimeHook::Malloc, "map2check_map_malloc", fn(v, {p, i32})},
      {RuntimeHook::Calloc, "map2check_map_calloc", fn(v, {p, i32, i32})},
      {RuntimeHook::Alloca, "map2check_map_alloca", fn(v, {p, p, i32, i32, i32, i32})},
      {RuntimeHook::NonStaticAlloca, "map2check_map_non_static_alloca", fn(v, {p, p, i32, i32, i32, i32})},
      {RuntimeHook::FunctionAddr, "map2check_map_funct_address", fn(v, {p, p})},
      {RuntimeHook::StorePointer, "map2check_map_store_pointer", fn(v, {p, p, i32, p, i32, p})},
      {RuntimeHook::Load, "map2check_check_load", fn(v, {p, i32, i32, i32, p})},
      {RuntimeHook::CheckFree, "map2check_check_free", fn(v, {p, p, p, i32, i32, p})},
      {RuntimeHook::MapFree, "map2check_map_free", fn(v, {p, p, p, i32, i32, p})},
      {RuntimeHook::FreeResolved, "map2check_check_free_resolved_address", fn(v, {p, i32, p, i16})},
      {RuntimeHook::PosixMemalign, "map2check_map_posix_memalign", fn(v, {p, i32})},
      {RuntimeHook::WasmMalloc, "map2check_wasm_malloc", fn(v, {i64, i64})},
      {RuntimeHook::WasmFree, "map2check_wasm_free", fn(v, {i64, i64})},
      {RuntimeHook::WasmCheckAccess, "map2check_wasm_check_access", fn(v, {i64, i64})},
  };
  ASSERT_EQ(expected.size(), allHooks().size());
  for (const Expected& e : expected) {
    RuntimeCall call = runtimeCall(e.hook, ctx);
    EXPECT_STREQ(call.name, e.name);
    EXPECT_EQ(call.type, e.type) << e.name;
  }
}

TEST(RuntimeABI, NonDetLoggerTypeIsUniform) {
  llvm::LLVMContext ctx;
  llvm::Type* i32 = llvm::Type::getInt32Ty(ctx);
  llvm::Type* p = llvm::PointerType::get(ctx, 0);
  // (line, scope, type_var, value*, function_name) -> non_det_log_t*
  EXPECT_EQ(Map2Check::nonDetLoggerType(ctx),
            llvm::FunctionType::get(p, {i32, i32, i32, p, p}, false));
}

// The alloca type of each nondet logger is the C type of docs/api-deltas.md
// of the library (pointer: void **, pchar: char **, bool: int), and type_var
// is develop's enum NONDET_TYPE (Map2CheckTypes.h:41-58).
TEST(RuntimeABI, NonDetValueSlotsFollowApiDeltas) {
  llvm::LLVMContext ctx;
  llvm::Type* i8 = llvm::Type::getInt8Ty(ctx);
  llvm::Type* i16 = llvm::Type::getInt16Ty(ctx);
  llvm::Type* i32 = llvm::Type::getInt32Ty(ctx);
  llvm::Type* i64 = llvm::Type::getInt64Ty(ctx);
  llvm::Type* p = llvm::PointerType::get(ctx, 0);
  llvm::Type* dbl = llvm::Type::getDoubleTy(ctx);
  struct Expected {
    const char* cType;
    llvm::Type* type;
    bool isSigned;
    int typeVar;
  };
  const std::vector<Expected> expected = {
      {"int", i32, true, 0},        {"char", i8, true, 1},
      {"pointer", p, false, 2},     {"ushort", i16, false, 3},
      {"long", i64, true, 4},       {"unsigned", i32, false, 5},
      {"ulong", i64, false, 6},     {"bool", i32, false, 7},
      {"uchar", i8, false, 8},      {"pchar", p, false, 9},
      {"size_t", i64, false, 10},   {"loff_t", i64, true, 11},
      {"sector_t", i64, false, 12}, {"uint", i32, false, 13},
      {"short", i16, true, 14},     {"double", dbl, true, 15},
      // Map2Check 9.0's FLOAT (issue #74): logged as a float, not a double.
      {"float", llvm::Type::getFloatTy(ctx), true, 16},
      {"longlong", i64, true, 17},  {"ulonglong", i64, false, 18},
      {"u8", i8, false, 19},        {"u16", i16, false, 20},
      {"u32", i32, false, 21},
  };
  for (const Expected& e : expected) {
    Map2Check::NonDetValueSlot slot = Map2Check::nonDetValueSlot(e.cType, ctx);
    EXPECT_EQ(slot.type, e.type) << e.cType;
    EXPECT_EQ(slot.isSigned, e.isSigned) << e.cType;
    EXPECT_EQ(slot.typeVar, e.typeVar) << e.cType;
    // Every type with a slot has a logger and a generator in the mapping.
    EXPECT_FALSE(nonDetLogger(e.cType).empty()) << e.cType;
    EXPECT_FALSE(nonDetGenerator(e.cType).empty()) << e.cType;
  }
  EXPECT_EQ(Map2Check::nonDetValueSlot("uint128", ctx).type, nullptr);
}

// The lists of program definitions that give way or become internal: the
// assume and the 22 generators the library exports; the SV-COMP and
// unprefixed runtime names. No name is in both.
TEST(RuntimeABI, ProgramDefinitionLists) {
  const std::vector<std::string>& discarded = Map2Check::discardedProgramDefinitions();
  const std::vector<std::string>& internalized =
      Map2Check::internalizedProgramDefinitions();
  EXPECT_EQ(discarded.size(), 23u);
  EXPECT_EQ(internalized.size(), 8u);
  std::set<std::string> all(discarded.begin(), discarded.end());
  all.insert(internalized.begin(), internalized.end());
  EXPECT_EQ(all.size(), discarded.size() + internalized.size());
  EXPECT_TRUE(all.count("__VERIFIER_assume"));
  EXPECT_TRUE(all.count("__VERIFIER_nondet_pchar"));
  EXPECT_TRUE(all.count("__VERIFIER_assert"));
  EXPECT_TRUE(all.count("vcc_reset_meta_data"));
  // Every generator the pass logs is discarded when the program defines it.
  for (const char* type : {"int", "uint", "char", "uchar", "short", "ushort",
                           "long", "ulong", "bool", "pointer", "pchar",
                           "size_t", "loff_t", "sector_t", "double", "float",
                           "longlong", "ulonglong", "u8", "u16", "u32"}) {
    EXPECT_TRUE(std::count(discarded.begin(), discarded.end(),
                           Map2Check::nonDetGenerator(type)))
        << type;
  }
}
