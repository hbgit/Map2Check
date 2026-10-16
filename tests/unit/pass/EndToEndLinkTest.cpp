/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

// Task 3.2, Steps 5 and 5b: the instrumented program links against the Rust
// runtime (libmap2check.a) with nothing left undefined or defined twice, in
// every analysis mode, and an assume rejection unwinds through it at -O2
// under the libFuzzer driver.
//
// Each input of inputs/ is compiled as the frontend compiles the program
// (Caller::compileFile: -c -emit-llvm -g -fexceptions -O0), instrumented with
// every pass of the mode as Caller::callPass chains them (the table below
// mirrors modules/frontend/caller.cpp, Caller::callPass; keep them in step
// until Task 3.3 shares it), and linked with
// clang-16 -fexceptions -fsanitize=fuzzer.

#include <gtest/gtest.h>

#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "../../../modules/frontend/utils/clang.hpp"

namespace {

namespace fs = std::filesystem;

/// The pipeline Caller::callPass builds for each mode: nondet-pass, the
/// mode's pass, then map2check-library with the mode's runtime number.
struct ModePipeline {
  const char* mode;  // -m2c-analysis-mode= (runtimeAnalysisMode)
  std::vector<std::string> plugins;
  std::string passes;
  std::string extraFlags;
};

const std::vector<ModePipeline>& pipelines() {
  static const std::vector<ModePipeline> all = {
      {"memtrack", {"NonDetPass", "MemoryTrackPass"}, "nondet-pass,memory-track",
       "-m2c-entry-function=main"},
      {"memcleanup", {"NonDetPass", "MemoryTrackPass"},
       "nondet-pass,memory-track", "-m2c-entry-function=main"},
      {"overflow", {"NonDetPass", "OverflowPass"}, "nondet-pass,overflow-pass", ""},
      {"assert", {"NonDetPass", "AssertPass"}, "nondet-pass,assert-pass", ""},
      {"target", {"NonDetPass", "TargetPass"}, "nondet-pass,target-pass",
       "-function-name=reach_error"},
      {"none", {"NonDetPass"}, "nondet-pass", ""},
  };
  return all;
}

const ModePipeline& pipeline(const std::string& mode) {
  for (const ModePipeline& p : pipelines()) {
    if (mode == p.mode) return p;
  }
  return pipelines().front();
}

std::vector<std::string> inputs() {
  std::vector<std::string> names;
  for (const fs::directory_entry& entry : fs::directory_iterator(M2C_TEST_INPUTS)) {
    std::string ext = entry.path().extension().string();
    if (ext == ".c" || ext == ".ll") names.push_back(entry.path().filename().string());
  }
  std::sort(names.begin(), names.end());
  return names;
}

std::string freshWorkDir(const std::string& name) {
  fs::path dir = fs::path(M2C_TEST_WORKDIR) / name;
  fs::remove_all(dir);
  fs::create_directories(dir);
  return dir.string();
}

int run(const std::string& command) {
  int rc = std::system(command.c_str());
  if (rc != 0) llvm::errs() << "command failed (" << rc << "): " << command << "\n";
  return rc;
}

std::string readFile(const std::string& path) {
  std::ifstream in(path);
  return std::string(std::istreambuf_iterator<char>(in), {});
}

/// Compiles inputs/<input> as the frontend does and instruments it with the
/// mode's pipeline. Returns the instrumented bitcode, or "" on error.
std::string instrument(const std::string& dir, const std::string& input,
                       const ModePipeline& p) {
  std::string compiled = dir + "/compiled.bc";
  std::ostringstream cc;
  cc << M2C_TEST_CLANG << " -Wno-everything -Winteger-overflow -c -emit-llvm -g "
     << Map2Check::kProgramUnwindFlags << " -O0 -o " << compiled << " "
     << M2C_TEST_INPUTS << "/" << input << " 2> " << dir << "/clang.err";
  if (run(cc.str()) != 0) return "";

  std::string output = dir + "/output.bc";
  std::ostringstream opt;
  opt << M2C_TEST_OPT << " -tailcallopt";
  for (const std::string& plugin : p.plugins) {
    opt << " -load-pass-plugin=" << M2C_PASS_PLUGIN_DIR << "/lib" << plugin
        << M2C_PASS_PLUGIN_SUFFIX;
  }
  opt << " -load-pass-plugin=" << M2C_PASS_PLUGIN_DIR << "/libMap2CheckLibrary"
      << M2C_PASS_PLUGIN_SUFFIX << " -entry-function=main"
      << " -m2c-analysis-mode=" << p.mode << " " << p.extraFlags << " -passes='"
      << p.passes << ",map2check-library' < " << compiled << " > " << output
      << " 2> " << dir << "/opt.err";
  if (run(opt.str()) != 0) return "";
  return output;
}

/// Links `bitcode` against the Rust runtime under the libFuzzer driver.
/// Returns clang's exit code; the link output goes to <dir>/link.txt.
int link(const std::string& dir, const std::string& bitcode,
         const std::string& optLevel) {
  std::ostringstream ld;
  ld << M2C_TEST_CLANG << " " << optLevel << " " << Map2Check::kProgramUnwindFlags
     << " -fsanitize=fuzzer " << bitcode << " " << M2C_RUST_LIB
     << " -ldl -lpthread -lm -o " << dir << "/program > " << dir
     << "/link.txt 2>&1";
  return std::system(ld.str().c_str());
}

/// Every distinct "undefined reference to `x'" / "multiple definition of
/// `x'" in the linker output.
std::set<std::string> linkerErrors(const std::string& log) {
  std::set<std::string> found;
  static const std::regex error(
      "(undefined reference to|multiple definition of) `[^']*'");
  for (std::sregex_iterator it(log.begin(), log.end(), error), end; it != end;
       ++it) {
    found.insert(it->str());
  }
  return found;
}

/// Writes `value` as the 4 little-endian bytes __VERIFIER_nondet_int reads.
void writeInt(const std::string& path, int32_t value) {
  std::ofstream out(path, std::ios::binary);
  unsigned char bytes[4];
  for (int b = 0; b < 4; ++b) {
    bytes[b] = static_cast<unsigned char>((static_cast<uint32_t>(value) >> (8 * b)) & 0xff);
  }
  out.write(reinterpret_cast<const char*>(bytes), 4);
}

struct LinkCase {
  std::string mode;
  std::string input;
};

void PrintTo(const LinkCase& c, std::ostream* os) { *os << c.mode << "/" << c.input; }

std::vector<LinkCase> linkCases() {
  std::vector<LinkCase> cases;
  for (const ModePipeline& p : pipelines()) {
    for (const std::string& input : inputs()) cases.push_back({p.mode, input});
  }
  return cases;
}

}  // namespace

class EndToEndLink : public ::testing::TestWithParam<LinkCase> {};

TEST_P(EndToEndLink, NoUndefinedOrDuplicateSymbol) {
  const LinkCase& c = GetParam();
  std::string name = c.mode + "-" + c.input;
  std::replace(name.begin(), name.end(), '.', '_');
  std::string dir = freshWorkDir(name);
  std::string bitcode = instrument(dir, c.input, pipeline(c.mode));
  ASSERT_FALSE(bitcode.empty()) << readFile(dir + "/clang.err")
                                << readFile(dir + "/opt.err");
  int rc = link(dir, bitcode, "-O0");
  std::string log = readFile(dir + "/link.txt");
  std::set<std::string> errors = linkerErrors(log);
  for (const std::string& e : errors) ADD_FAILURE() << e;
  EXPECT_EQ(rc, 0) << log;
  EXPECT_TRUE(fs::exists(dir + "/program"));
}

INSTANTIATE_TEST_SUITE_P(
    PassIR, EndToEndLink, ::testing::ValuesIn(linkCases()),
    [](const ::testing::TestParamInfo<LinkCase>& info) {
      std::string name = info.param.mode + "_" + info.param.input;
      for (char& ch : name) {
        if (!std::isalnum(static_cast<unsigned char>(ch))) ch = '_';
      }
      return name;
    });

// Step 5b (D1 condition 11): the program bitcode keeps a defined unwind, and
// the -O2 binary rejects 100 inputs through __VERIFIER_assume, unwinding
// through a C frame each time, then runs an accepted input to the end, with
// no abort.
TEST(EndToEndRun, AssumeRejectionsUnwindAtO2UnderLibFuzzer) {
  std::string dir = freshWorkDir("assume-runs-O2");
  std::string bitcode = instrument(dir, "assume_runs.c", pipeline("assert"));
  ASSERT_FALSE(bitcode.empty()) << readFile(dir + "/opt.err");

  // The frames the unwind crosses are not nounwind.
  static llvm::LLVMContext ctx;
  llvm::SMDiagnostic err;
  std::unique_ptr<llvm::Module> m = llvm::parseIRFile(bitcode, err, ctx);
  ASSERT_NE(m, nullptr);
  ASSERT_FALSE(llvm::verifyModule(*m, &llvm::errs()));
  for (const char* fn : {"__map2check_main__", "step", "__VERIFIER_assume"}) {
    llvm::Function* f = m->getFunction(fn);
    ASSERT_NE(f, nullptr) << fn;
    EXPECT_FALSE(f->hasFnAttribute(llvm::Attribute::NoUnwind)) << fn;
  }

  ASSERT_EQ(link(dir, bitcode, "-O2"), 0) << readFile(dir + "/link.txt");

  // x <= 0 is rejected by __VERIFIER_assume(x > 0); x = 5 is accepted.
  fs::create_directories(dir + "/inputs");
  std::ostringstream files;
  for (int k = 0; k < 100; ++k) {
    std::string path = dir + "/inputs/rejected" + std::to_string(k);
    writeInt(path, -k);
    files << " " << path;
  }
  writeInt(dir + "/inputs/accepted", 5);
  files << " " << dir << "/inputs/accepted";

  std::ostringstream cmd;
  cmd << dir << "/program" << files.str() << " > " << dir << "/run.out 2> "
      << dir << "/run.err";
  int rc = std::system(cmd.str().c_str());
  std::string out = readFile(dir + "/run.out");
  std::string errText = readFile(dir + "/run.err");
  EXPECT_EQ(rc, 0) << errText;

  // libFuzzer ran all 101 inputs, in order, in one process.
  size_t executed = 0;
  for (size_t pos = errText.find("Executed "); pos != std::string::npos;
       pos = errText.find("Executed ", pos + 1)) {
    ++executed;
  }
  EXPECT_EQ(executed, 101u) << errText;
  // Only the accepted input got past the assume.
  EXPECT_EQ(out, "accepted 4\n");
  for (const char* bad : {"deadly signal", "failed to initiate panic", "Aborted",
                          "panic in FFI boundary"}) {
    EXPECT_EQ(errText.find(bad), std::string::npos) << bad << "\n" << errText;
  }
}

// SV-COMP programs define library exports themselves (inputs/svcomp_defs.c).
// At run time the nondet value comes from the fuzzer input, not from the
// program's `return 0;` generator, and the assume is the library's (it
// prunes, where the program's would abort); the program's internalized
// get_current_step keeps its own body.
TEST(EndToEndRun, ProgramDefinedExportsGiveWayToTheLibrary) {
  std::string dir = freshWorkDir("svcomp-defs-O2");
  std::string bitcode = instrument(dir, "svcomp_defs.c", pipeline("assert"));
  ASSERT_FALSE(bitcode.empty()) << readFile(dir + "/opt.err");
  ASSERT_EQ(link(dir, bitcode, "-O2"), 0) << readFile(dir + "/link.txt");

  fs::create_directories(dir + "/inputs");
  writeInt(dir + "/inputs/rejected", 7);       // __VERIFIER_assume(x != 7)
  writeInt(dir + "/inputs/reached", 123456);   // if (x == 123456)
  std::ostringstream cmd;
  cmd << dir << "/program " << dir << "/inputs/rejected " << dir
      << "/inputs/reached > " << dir << "/run.out 2> " << dir << "/run.err";
  int rc = std::system(cmd.str().c_str());
  std::string out = readFile(dir + "/run.out");
  std::string errText = readFile(dir + "/run.err");
  EXPECT_EQ(rc, 0) << errText;
  EXPECT_NE(out.find("from the input 42\n"), std::string::npos) << out;
  for (const char* bad : {"deadly signal", "Aborted", "panic in FFI boundary"}) {
    EXPECT_EQ(errText.find(bad), std::string::npos) << bad << "\n" << errText;
  }
}
