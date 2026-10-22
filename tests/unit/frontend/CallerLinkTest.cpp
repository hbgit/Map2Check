/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

// How the frontend links the instrumented program with map2check-library
// (decision D1): the native libmap2check.a for AFL++ (route B1), the
// map2check-klee.bc bitcode for KLEE (route A'), and never a bitcode file of
// the old C runtime.

#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <set>
#include <string>
#include <vector>

#include "../../../modules/frontend/caller.hpp"

using Map2Check::Caller;
using Map2Check::DataStructure;
using Map2Check::Map2CheckMode;
using Map2Check::NonDetGenerator;
using Map2Check::buildFuzzerCommand;
using Map2Check::buildLinkCommand;

namespace fs = std::filesystem;

namespace {

const Map2CheckMode kAllModes[] = {
    Map2CheckMode::MEMTRACK_MODE,     Map2CheckMode::MEMCLEANUP_MODE,
    Map2CheckMode::OVERFLOW_MODE,     Map2CheckMode::ASSERT_MODE,
    Map2CheckMode::REACHABILITY_MODE, Map2CheckMode::COVER_BRANCHES_MODE};

const NonDetGenerator kAllGenerators[] = {
    NonDetGenerator::None, NonDetGenerator::AFLPlusPlus, NonDetGenerator::Klee};

// Every bitcode file of the C runtime that linkLLVM used to pass to llvm-link.
const char* const kCRuntimeBitcode[] = {
    "Map2CheckFunctions.bc",   "TrackBBLog.bc",
    "NonDetLog.bc",            "PropertyGenerator.bc",
    "ContainerRealloc.bc",     "ContainerBTree.bc",
    "BTree.bc",                "AnalysisModeMemtrack.bc",
    "AnalysisModeMemcleanup.bc", "AnalysisModeOverflow.bc",
    "AnalysisModeAssert.bc",   "AnalysisModeNone.bc",
    "AllocationLog.bc",        "ListLog.bc",
    "HeapLog.bc",              "NonDetGeneratorNone.bc",
    "NonDetGeneratorKlee.bc",  "NonDetGeneratorAFL.bc",
    "WasmRuntimeStubs.bc",     "WitnessGeneration.bc",
    "WitnessGenerationNone.bc"};

size_t occurrences(const std::string& haystack, const std::string& needle) {
  size_t count = 0;
  for (size_t at = haystack.find(needle); at != std::string::npos;
       at = haystack.find(needle, at + needle.size()))
    ++count;
  return count;
}

bool contains(const std::string& haystack, const std::string& needle) {
  return haystack.find(needle) != std::string::npos;
}

// A fresh, empty directory under the system temp dir.
std::string freshDir(const std::string& tag) {
  std::string pattern =
      (fs::temp_directory_path() / ("callerlink-" + tag + "-XXXXXX")).string();
  std::vector<char> buffer(pattern.begin(), pattern.end());
  buffer.push_back('\0');
  const char* made = mkdtemp(buffer.data());
  EXPECT_NE(made, nullptr);
  return made != nullptr ? std::string(made) : std::string();
}

}  // namespace

TEST(CallerLink, LinksOnlyTheRustRuntime) {
  Caller::LinkInputs in{"h", Map2CheckMode::MEMTRACK_MODE,
                        NonDetGenerator::AFLPlusPlus, DataStructure::Array,
                        /*wasm=*/false, /*witness=*/false};
  std::string link = buildLinkCommand(in);
  EXPECT_FALSE(link.empty());
  EXPECT_EQ(link.find("${MAP2CHECK_PATH}/lib/"), std::string::npos) << link;
  EXPECT_NE(buildFuzzerCommand(in).find("libmap2check.a"), std::string::npos);
}

TEST(CallerLink, EveryModeAndGeneratorLinksRuntime) {
  for (auto mode : kAllModes)
    for (auto gen : {NonDetGenerator::AFLPlusPlus, NonDetGenerator::None})
      for (bool wasm : {false, true})
        for (bool witness : {false, true})
          for (bool cmplog : {false, true}) {
            if (witness && cmplog) continue;  // CmpLog is never a witness
            Caller::LinkInputs in{"h", mode, gen, DataStructure::Array,
                                  wasm, witness, cmplog};
            const std::string fuzz = buildFuzzerCommand(in);
            EXPECT_TRUE(contains(fuzz, "${MAP2CHECK_PATH}/lib/libmap2check.a"))
                << fuzz;
            // The system libraries follow the archive that needs them.
            EXPECT_TRUE(contains(
                fuzz, "${MAP2CHECK_PATH}/lib/libmap2check.a -ldl -lpthread -lm"))
                << fuzz;
            EXPECT_TRUE(contains(fuzz, "-fsanitize=fuzzer")) << fuzz;
            EXPECT_TRUE(contains(fuzz, " " + Map2Check::resultBitcode(in) + " "))
                << fuzz;
            EXPECT_TRUE(contains(fuzz, " -o " + Map2Check::fuzzerBinary(in)))
                << fuzz;
          }
}

TEST(CallerLink, NoCRuntimeBitcodeIsLinkedInAnyConfiguration) {
  for (auto mode : kAllModes)
    for (auto gen : kAllGenerators)
      for (auto ds : {DataStructure::Array, DataStructure::BTree})
        for (bool wasm : {false, true})
          for (bool witness : {false, true}) {
            Caller::LinkInputs in{"h", mode, gen, ds, wasm, witness};
            const std::string link = buildLinkCommand(in);
            const std::string fuzz = buildFuzzerCommand(in);
            for (const char* bc : kCRuntimeBitcode) {
              EXPECT_FALSE(contains(link, bc)) << link;
              EXPECT_FALSE(contains(fuzz, bc)) << fuzz;
            }
            // The one installed file a link may name is the KLEE runtime,
            // and only for KLEE.
            const size_t expected = gen == NonDetGenerator::Klee ? 1 : 0;
            EXPECT_EQ(occurrences(link, "${MAP2CHECK_PATH}/lib/"), expected)
                << link;
            EXPECT_EQ(occurrences(link, Map2Check::kRustKleeRuntime), expected)
                << link;
            EXPECT_TRUE(contains(link, " h-output.bc")) << link;
            EXPECT_TRUE(contains(link, " -o " + Map2Check::resultBitcode(in)))
                << link;
          }
}

TEST(CallerLink, ResultAndBinaryNames) {
  Caller::LinkInputs in{"h", Map2CheckMode::ASSERT_MODE,
                        NonDetGenerator::AFLPlusPlus, DataStructure::Array,
                        false, false};
  EXPECT_EQ(Map2Check::resultBitcode(in), "h-result.bc");
  EXPECT_EQ(Map2Check::fuzzerBinary(in), "h-fuzzed.out");
  in.witness = true;
  EXPECT_EQ(Map2Check::resultBitcode(in), "h-witness-result.bc");
  EXPECT_EQ(Map2Check::fuzzerBinary(in), "h-witness-fuzzed.out");
  in.witness = false;
  in.cmplog = true;
  EXPECT_EQ(Map2Check::resultBitcode(in), "h-result.bc");
  EXPECT_EQ(Map2Check::fuzzerBinary(in), "h-cmplog.out");
}

TEST(CallerLink, FuzzerFlagsAreKeptAndCmplogIsAnEnvPrefix) {
  Caller::LinkInputs in{"h", Map2CheckMode::OVERFLOW_MODE,
                        NonDetGenerator::AFLPlusPlus, DataStructure::Array,
                        false, false};
  const std::string fuzzed = buildFuzzerCommand(in);
  EXPECT_TRUE(contains(fuzzed, " -g ")) << fuzzed;
  EXPECT_TRUE(contains(fuzzed, " -O2 ")) << fuzzed;
  EXPECT_FALSE(contains(fuzzed, "AFL_LLVM_CMPLOG")) << fuzzed;

  in.cmplog = true;
  const std::string cmplog = buildFuzzerCommand(in);
  // `env` so that the command can be run after `timeout`.
  EXPECT_EQ(cmplog.rfind("env AFL_LLVM_CMPLOG=1 ", 0), 0u) << cmplog;
  EXPECT_TRUE(contains(cmplog, " -O2 ")) << cmplog;

  in.cmplog = false;
  in.witness = true;
  const std::string witness = buildFuzzerCommand(in);
  EXPECT_TRUE(contains(witness, " -g ")) << witness;
  EXPECT_FALSE(contains(witness, "AFL_LLVM_CMPLOG")) << witness;
}

TEST(CallerLink, WitnessConstructorIsLinkedOnlyIntoWitnessArtifacts) {
  const std::string source = Map2Check::witnessConstructorSource();
  EXPECT_TRUE(contains(source, "__attribute__((constructor))")) << source;
  EXPECT_TRUE(contains(source, "map2check_enable_witness();")) << source;
  EXPECT_TRUE(contains(source, "void map2check_enable_witness(void);"))
      << source;

  for (auto mode : kAllModes)
    for (bool wasm : {false, true}) {
      // AFL++: a native object in the witness fuzzer binary only.
      Caller::LinkInputs afl{"h", mode, NonDetGenerator::AFLPlusPlus,
                             DataStructure::Array, wasm, true};
      const std::string object = Map2Check::witnessObjectPath(afl);
      EXPECT_EQ(object, "h-witness.o");
      EXPECT_TRUE(contains(buildFuzzerCommand(afl), " " + object + " "));
      EXPECT_FALSE(contains(buildLinkCommand(afl), "h-witness."));
      afl.witness = false;
      EXPECT_FALSE(contains(buildFuzzerCommand(afl), "h-witness."));
      afl.cmplog = true;
      EXPECT_FALSE(contains(buildFuzzerCommand(afl), "h-witness."));

      // KLEE: bitcode linked into the witness bitcode only.
      Caller::LinkInputs klee{"h", mode, NonDetGenerator::Klee,
                              DataStructure::Array, wasm, true};
      const std::string bitcode = Map2Check::witnessObjectPath(klee);
      EXPECT_EQ(bitcode, "h-witness.bc");
      EXPECT_TRUE(contains(buildLinkCommand(klee), " " + bitcode + " "))
          << buildLinkCommand(klee);
      klee.witness = false;
      EXPECT_FALSE(contains(buildLinkCommand(klee), "h-witness."))
          << buildLinkCommand(klee);
    }
}

TEST(CallerLink, WitnessObjectIsNativeForAflAndBitcodeForKlee) {
  Caller::LinkInputs afl{"h", Map2CheckMode::MEMTRACK_MODE,
                         NonDetGenerator::AFLPlusPlus, DataStructure::Array,
                         false, true};
  Caller::LinkInputs klee = afl;
  klee.generator = NonDetGenerator::Klee;
  const std::string native = Map2Check::buildWitnessObjectCommand(afl);
  const std::string bitcode = Map2Check::buildWitnessObjectCommand(klee);
  for (const std::string& cmd : {native, bitcode}) {
    EXPECT_TRUE(contains(cmd, " -c ")) << cmd;
    EXPECT_TRUE(contains(cmd, " -fexceptions")) << cmd;
    EXPECT_TRUE(contains(cmd, " h-witness.c")) << cmd;
  }
  EXPECT_FALSE(contains(native, "-emit-llvm")) << native;
  EXPECT_TRUE(contains(native, " -o h-witness.o")) << native;
  EXPECT_TRUE(contains(bitcode, " -emit-llvm")) << bitcode;
  EXPECT_TRUE(contains(bitcode, " -o h-witness.bc")) << bitcode;
  EXPECT_EQ(Map2Check::witnessSourcePath(afl), "h-witness.c");
}

TEST(CallerLink, KleeLinksTheKleeRuntimeBitcode) {
  Caller::LinkInputs in{"h", Map2CheckMode::MEMTRACK_MODE,
                        NonDetGenerator::Klee, DataStructure::Array, false,
                        true};
  const std::string link = buildLinkCommand(in);
  EXPECT_TRUE(contains(link, "llvm-link")) << link;
  EXPECT_TRUE(contains(link, " ${MAP2CHECK_PATH}/lib/map2check-klee.bc"))
      << link;
  EXPECT_TRUE(contains(link, " -o h-witness-result.bc")) << link;
  EXPECT_FALSE(contains(link, "libmap2check.a")) << link;
}

TEST(CallerLink, ReplayPassesTheInputAsAnArgument) {
  const std::string file = "afl-out/default/crashes/id:000000,sig:06,src:000000";
  const std::string replay = Map2Check::buildReplayCommand("h", file);
  EXPECT_EQ(replay.find('<'), std::string::npos) << replay;
  EXPECT_EQ(replay, "./h-witness-fuzzed.out '" + file + "'");
  // A quote in the name cannot break out of the argument.
  EXPECT_EQ(Map2Check::buildReplayCommand("h", "a'b"),
            "./h-witness-fuzzed.out 'a'\\''b'");
}

TEST(CallerLink, PlaceholderSeedHasAtLeastEightBytes) {
  const std::string root = freshDir("seed");
  const std::string dir = root + "/afl-in";  // does not exist yet
  ASSERT_TRUE(Map2Check::prepareAflInputDir(dir));
  std::vector<fs::path> seeds(fs::directory_iterator(dir),
                              fs::directory_iterator{});
  ASSERT_EQ(seeds.size(), 1u);
  EXPECT_GE(fs::file_size(seeds[0]), Map2Check::kAflSeedMinBytes);
  EXPECT_GE(Map2Check::kAflSeedMinBytes, 8u);
  fs::remove_all(root);
}

TEST(CallerLink, ExistingSeedsAreLeftUntouched) {
  const std::string dir = freshDir("exchange");
  {
    std::ofstream(dir + "/klee-0", std::ios::binary) << "AB";
  }
  ASSERT_TRUE(Map2Check::prepareAflInputDir(dir));
  std::set<std::string> names;
  for (const auto& entry : fs::directory_iterator(dir))
    names.insert(entry.path().filename().string());
  EXPECT_EQ(names, std::set<std::string>{"klee-0"});
  std::ifstream in(dir + "/klee-0", std::ios::binary);
  std::string content((std::istreambuf_iterator<char>(in)),
                      std::istreambuf_iterator<char>());
  EXPECT_EQ(content, "AB");
  fs::remove_all(dir);
}

TEST(CallerLink, KleeRuntimeIsLookedUpUnderMap2CheckPath) {
  // Restores MAP2CHECK_PATH however the test ends.
  struct RestoreEnv {
    bool had;
    std::string value;
    ~RestoreEnv() {
      if (had)
        setenv("MAP2CHECK_PATH", value.c_str(), 1);
      else
        unsetenv("MAP2CHECK_PATH");
    }
  };
  const char* previous = getenv("MAP2CHECK_PATH");
  RestoreEnv restore{previous != nullptr,
                     previous != nullptr ? previous : ""};
  const std::string prefix = freshDir("prefix");
  ASSERT_EQ(setenv("MAP2CHECK_PATH", prefix.c_str(), 1), 0);
  EXPECT_EQ(Map2Check::installedKleeRuntime(),
            prefix + "/lib/map2check-klee.bc");
  EXPECT_FALSE(Map2Check::kleeRuntimeInstalled());
  fs::create_directories(prefix + "/lib");
  std::ofstream(prefix + "/lib/map2check-klee.bc") << "BC";
  EXPECT_TRUE(Map2Check::kleeRuntimeInstalled());
  fs::remove_all(prefix);
}

TEST(CallerLink, BtreeIsIgnoredWithOneWarning) {
  testing::internal::CaptureStdout();
  Map2Check::warnBtreeIgnored();
  const std::string out = testing::internal::GetCapturedStdout();
  EXPECT_EQ(occurrences(out, "--btree is ignored by the Rust runtime"), 1u)
      << out;
  for (auto gen : kAllGenerators) {
    Caller::LinkInputs array{"h", Map2CheckMode::MEMTRACK_MODE, gen,
                             DataStructure::Array, false, true};
    Caller::LinkInputs btree = array;
    btree.dataStructure = DataStructure::BTree;
    EXPECT_EQ(buildLinkCommand(array), buildLinkCommand(btree));
    EXPECT_EQ(buildFuzzerCommand(array), buildFuzzerCommand(btree));
  }
}
