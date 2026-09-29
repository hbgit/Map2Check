/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

#include "../../../modules/frontend/utils/slicer.hpp"

// The suite is generated on the slice and run by TestCov on the ORIGINAL
// program, so every nondet read the original performs must survive slicing.
TEST(SlicingCriteria, AppendsEveryNondetFunctionAfterThePrimary) {
  const std::string criteria = Map2Check::slicingCriteria({"reach_error"});
  EXPECT_EQ(criteria.rfind("reach_error,", 0), 0u);
  for (const std::string& name : Map2Check::nondetFunctionNames()) {
    EXPECT_NE(criteria.find("," + name), std::string::npos) << name;
  }
}

TEST(SlicingCriteria, KeepsSeveralPrimariesInOrder) {
  const std::string criteria =
      Map2Check::slicingCriteria({"__VERIFIER_assert", "__assert_fail"});
  EXPECT_EQ(criteria.rfind("__VERIFIER_assert,__assert_fail,", 0), 0u);
}

TEST(NondetFunctionNames, CoversWhatNonDetPassInstruments) {
  const auto& names = Map2Check::nondetFunctionNames();
  for (const char* type : {"bool", "char", "uchar", "short", "ushort", "int",
                           "uint", "unsigned", "long", "ulong", "size_t",
                           "loff_t", "sector_t", "pointer", "pchar", "double"}) {
    const std::string name = std::string("__VERIFIER_nondet_") + type;
    EXPECT_NE(std::find(names.begin(), names.end(), name), names.end()) << name;
  }
}

TEST(TargetStubSource, VoidTargetGetsAVoidStub) {
  EXPECT_EQ(Map2Check::targetStubSource("reach_error"),
            "void __attribute__((weak)) reach_error(void) {}\n");
}

// __VERIFIER_assert takes the condition; a (void) stub would not link against
// the program's own declaration.
TEST(TargetStubSource, AssertStubTakesTheCondition) {
  EXPECT_EQ(Map2Check::targetStubSource("__VERIFIER_assert"),
            "void __attribute__((weak)) __VERIFIER_assert(int cond) {}\n");
}

TEST(ParseSlicerStatistics, ReadsBeforeAndAfter) {
  const std::string output =
      "Statistics before Globals/Functions/Blocks/Instr.: 37 97 2215 10764\n"
      "[llvm-slicer] Sliced away 1454 from 4227 nodes in DG\n"
      "Statistics after Globals/Functions/Blocks/Instr.: 37 38 444 2989\n";
  const Map2Check::SlicerStatistics stats =
      Map2Check::parseSlicerStatistics(output);
  ASSERT_TRUE(stats.found);
  EXPECT_EQ(stats.before.functions, 97u);
  EXPECT_EQ(stats.before.blocks, 2215u);
  EXPECT_EQ(stats.before.instructions, 10764u);
  EXPECT_EQ(stats.after.globals, 37u);
  EXPECT_EQ(stats.after.functions, 38u);
  EXPECT_EQ(stats.after.instructions, 2989u);
}

// A slicer that prints no statistics must not be reported as having sliced
// everything away.
TEST(ParseSlicerStatistics, MissingLinesAreNotFound) {
  EXPECT_FALSE(Map2Check::parseSlicerStatistics("").found);
  EXPECT_FALSE(Map2Check::parseSlicerStatistics(
                   "Statistics before Globals/Functions/Blocks/Instr.: 1 2 3 4\n")
                   .found);
}

TEST(DescribeSlice, ReportsCountsWhenFound) {
  Map2Check::SlicerStatistics stats;
  stats.found = true;
  stats.before = {37, 97, 2215, 10764};
  stats.after = {37, 38, 444, 2989};
  EXPECT_EQ(Map2Check::describeSlice("reach_error", stats, 184164, 171708),
            "Sliced with respect to reach_error: 97/2215/10764 -> 38/444/2989 "
            "functions/blocks/instructions (184164 -> 171708 bytes of "
            "bitcode)");
}

TEST(DescribeSlice, FallsBackToBytesWithoutStatistics) {
  EXPECT_EQ(Map2Check::describeSlice("reach_error", {}, 10, 8),
            "Sliced with respect to reach_error: 10 -> 8 bytes of bitcode");
}

// The fixed list cannot know every name a benchmark declares (int128,
// uint128, ...); a name missing from the criteria silently reintroduces the
// shifted suite. The names come from the program itself as well.
TEST(NondetNamesInIR, FindsEveryDeclaredOrCalledNondetFunction) {
  const std::string ir =
      "declare i32 @__VERIFIER_nondet_int()\n"
      "declare i128 @__VERIFIER_nondet_int128()\n"
      "  %1 = call i128 @__VERIFIER_nondet_int128(), !dbg !19\n"
      "  call void @reach_error()\n"
      "@__VERIFIER_nondet_not_a_call = global i32 0\n";
  const std::vector<std::string> names = Map2Check::nondetNamesInIR(ir);
  ASSERT_EQ(names.size(), 3u);
  EXPECT_EQ(names[0], "__VERIFIER_nondet_int");
  EXPECT_EQ(names[1], "__VERIFIER_nondet_int128");
  EXPECT_EQ(names[2], "__VERIFIER_nondet_not_a_call");
}

TEST(SlicingCriteria, AddsNamesFromTheProgramOnceEach) {
  const std::string criteria = Map2Check::slicingCriteria(
      {"reach_error"}, {"__VERIFIER_nondet_int128", "__VERIFIER_nondet_int"});
  EXPECT_NE(criteria.find(",__VERIFIER_nondet_int128"), std::string::npos);
  size_t count = 0;
  for (size_t at = criteria.find("__VERIFIER_nondet_int,");
       at != std::string::npos;
       at = criteria.find("__VERIFIER_nondet_int,", at + 1)) {
    ++count;
  }
  EXPECT_EQ(count, 1u);
}

// After instrumentation the memory property lives in the runtime calls
// MemoryTrackPass inserted; every one of them is a criterion, so nothing that
// records memory is sliced away.
TEST(RuntimeNamesInIR, FindsEveryMap2checkSymbolOnce) {
  const std::string ir =
      "declare void @map2check_malloc(ptr, i64)\n"
      "  call void @map2check_check_deref(ptr %3, i64 4), !dbg !7\n"
      "  call void @map2check_malloc(ptr %1, i64 8)\n"
      "  call i32 @__VERIFIER_nondet_int()\n";
  const std::vector<std::string> names = Map2Check::runtimeNamesInIR(ir);
  ASSERT_EQ(names.size(), 2u);
  EXPECT_EQ(names[0], "map2check_malloc");
  EXPECT_EQ(names[1], "map2check_check_deref");
}

// A memory error can happen INSIDE an external function (strcpy overflowing a
// stack buffer). No map2check_* call depends on such a call, so without it as
// a criterion the slicer drops it and the bug with it -- measured: CASTLE-787-2
// went from UNKNOWN to a wrong TRUE. Every declared-but-undefined function is
// a criterion in the post-instrumentation modes; intrinsics are not calls.
TEST(ExternalNamesInIR, FindsDeclaredFunctionsButNotIntrinsicsOrDefinitions) {
  const std::string ir =
      "define dso_local i32 @__map2check_main__() {\n"
      "  call ptr @strcpy(ptr %1, ptr @.str)\n"
      "}\n"
      "declare ptr @strcpy(ptr noundef, ptr noundef) #2\n"
      "declare void @llvm.dbg.declare(metadata, metadata, metadata) #1\n"
      "declare i32 @printf(ptr noundef, ...) #2\n"
      "declare void @map2check_malloc(ptr, i64)\n"
      "declare ptr @strcpy(ptr noundef, ptr noundef) #2\n";
  const std::vector<std::string> names = Map2Check::externalNamesInIR(ir);
  ASSERT_EQ(names.size(), 3u);
  EXPECT_EQ(names[0], "strcpy");
  EXPECT_EQ(names[1], "printf");
  EXPECT_EQ(names[2], "map2check_malloc");
}

// clang lowers memcpy/memset/memmove (and struct copies) to intrinsics, and a
// copy that overflows into a buffer nothing reads again feeds no criterion:
// without its name the slicer drops it, the same wrong-TRUE shape as the
// strcpy of CASTLE-787-2. Other intrinsics (debug info) are not calls.
TEST(ExternalNamesInIR, KeepsTheMemoryIntrinsics) {
  const std::string ir =
      "declare void @llvm.memcpy.p0.p0.i64(ptr, ptr, i64, i1)\n"
      "declare void @llvm.memset.p0.i64(ptr, i8, i64, i1)\n"
      "declare void @llvm.memmove.p0.p0.i64(ptr, ptr, i64, i1)\n"
      "declare void @llvm.dbg.declare(metadata, metadata, metadata)\n"
      "declare void @llvm.lifetime.start.p0(i64, ptr)\n";
  const std::vector<std::string> names = Map2Check::externalNamesInIR(ir);
  ASSERT_EQ(names.size(), 3u);
  EXPECT_EQ(names[0], "llvm.memcpy.p0.p0.i64");
  EXPECT_EQ(names[1], "llvm.memset.p0.i64");
  EXPECT_EQ(names[2], "llvm.memmove.p0.p0.i64");
}

// Slicing the instrumented module is only sound with the runtime checks as
// criteria. If the IR could not be read (a failed disassembly, an empty file)
// there are none, and the slice would remove every check: refuse instead.
TEST(InstrumentedSliceCriteria, RefusesWithoutRuntimeCalls) {
  std::vector<std::string> criteria;
  EXPECT_FALSE(Map2Check::instrumentedSliceCriteria("", &criteria));
  EXPECT_FALSE(Map2Check::instrumentedSliceCriteria(
      "declare i32 @__VERIFIER_nondet_int()\n", &criteria));
}

TEST(InstrumentedSliceCriteria, CollectsRuntimeThenExternalNames) {
  std::vector<std::string> criteria;
  ASSERT_TRUE(Map2Check::instrumentedSliceCriteria(
      "  call void @map2check_check_deref(ptr %3, i64 4)\n"
      "declare ptr @strcpy(ptr, ptr)\n"
      "declare void @map2check_check_deref(ptr, i64)\n",
      &criteria));
  ASSERT_EQ(criteria.size(), 2u);
  EXPECT_EQ(criteria[0], "map2check_check_deref");
  EXPECT_EQ(criteria[1], "strcpy");
}

// --- the slice cache (tacas 2d) ----------------------------------------------

// Every hybrid phase recreates the scratch directory and used to slice again:
// on eca-* the slicer timed out in phase 1 AND phase 2, and the run blew its
// budget. The cache reuses a slice only for the very same input and settings.
TEST(SliceCacheKey, IsDeterministic) {
  EXPECT_EQ(Map2Check::sliceCacheKey("BC", "reach_error", "main", ""),
            Map2Check::sliceCacheKey("BC", "reach_error", "main", ""));
}

TEST(SliceCacheKey, ChangesWithEveryInput) {
  const std::string base =
      Map2Check::sliceCacheKey("BC", "reach_error", "main", "");
  EXPECT_NE(base, Map2Check::sliceCacheKey("BD", "reach_error", "main", ""));
  EXPECT_NE(base, Map2Check::sliceCacheKey("BC", "reach_errors", "main", ""));
  EXPECT_NE(base, Map2Check::sliceCacheKey("BC", "reach_error", "mai", ""));
  EXPECT_NE(base,
            Map2Check::sliceCacheKey("BC", "reach_error", "main", "--pta=fs"));
  // Field boundaries count: moving a character between fields is another key.
  EXPECT_NE(Map2Check::sliceCacheKey("B", "Creach_error", "main", ""), base);
}

TEST(SliceCacheKey, IsAFileName) {
  const std::string key =
      Map2Check::sliceCacheKey("BC", "reach_error", "main", "--cda=ntscd");
  EXPECT_EQ(key.size(), 16u);
  EXPECT_EQ(key.find_first_not_of("0123456789abcdef"), std::string::npos);
}

TEST(SliceCachePath, SitsBesideTheScratchDirectory) {
  EXPECT_EQ(Map2Check::sliceCachePath("/w", "abc.map2check"),
            "/w/abc.map2check.slice");
}

TEST(SliceCleanupPasses, MapsTheKnob) {
  EXPECT_EQ(Map2Check::sliceCleanupPasses(""), "");
  EXPECT_EQ(Map2Check::sliceCleanupPasses("none"), "");
  EXPECT_EQ(Map2Check::sliceCleanupPasses("light"),
            "-passes='function(simplifycfg,dce),globaldce'");
  EXPECT_EQ(Map2Check::sliceCleanupPasses("o2"), "-O2");
  EXPECT_EQ(Map2Check::sliceCleanupPasses("bogus"), "");
}
