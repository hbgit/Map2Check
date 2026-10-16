/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

// Linking the instrumented program with map2check-library (decision D1 in
// map2check-library's docs/cutover/decisoes.md):
//
//   afl / none  route B + B1: the instrumented bitcode is compiled natively by
//               afl-clang-fast -fsanitize=fuzzer and linked with the installed
//               libmap2check.a, whose LLVMFuzzerTestOneInput drives the
//               renamed __map2check_main__. No runtime bitcode is llvm-linked.
//   symex       route A': the instrumented bitcode is llvm-linked with
//               map2check-klee.bc (rustc 1.72.1 / LLVM 16), which defines the
//               main KLEE runs.
//
// The C runtime chose the property mode, the container, the generator and
// the witness behaviour by WHICH .bc files it linked. The Rust runtime is one
// library: the mode comes from map2check_set_mode (Map2CheckLibrary pass),
// there is one container, and the witness is switched on at run time by a
// constructor that calls map2check_enable_witness (Step 4b), linked only into
// the witness artefacts as WitnessGeneration.bc used to be.

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <system_error>

#include "caller.hpp"
#include "utils/clang.hpp"
#include "utils/log.hpp"
#include "utils/tools.hpp"

namespace Map2Check {

std::string resultBitcode(const Caller::LinkInputs& in) {
  return in.hash + (in.witness ? "-witness-result.bc" : "-result.bc");
}

std::string witnessSourcePath(const Caller::LinkInputs& in) {
  return in.hash + "-witness.c";
}

std::string witnessObjectPath(const Caller::LinkInputs& in) {
  return in.hash +
         (in.generator == NonDetGenerator::Klee ? "-witness.bc" : "-witness.o");
}

std::string witnessConstructorSource() {
  return "void map2check_enable_witness(void);\n"
         "__attribute__((constructor)) static void m2c_witness(void) {\n"
         "  map2check_enable_witness();\n"
         "}\n";
}

std::string buildWitnessObjectCommand(const Caller::LinkInputs& in) {
  std::ostringstream command;
  command << clangBinary << " -c";
  if (in.generator == NonDetGenerator::Klee) command << " -emit-llvm";
  command << " " << kProgramUnwindFlags << " -o " << witnessObjectPath(in)
          << " " << witnessSourcePath(in);
  return command.str();
}

std::string buildLinkCommand(const Caller::LinkInputs& in) {
  std::ostringstream command;
  command << llvmLinkBinary << " " << in.hash << "-output.bc";
  if (in.generator == NonDetGenerator::Klee) {
    command << " " << kRustKleeRuntime;
    if (in.witness) command << " " << witnessObjectPath(in);
  }
  command << " -o " << resultBitcode(in);
  return command.str();
}

std::string fuzzerBinary(const Caller::LinkInputs& in) {
  if (in.cmplog) return in.hash + "-cmplog.out";
  return in.hash + (in.witness ? "-witness-fuzzed.out" : "-fuzzed.out");
}

std::string buildFuzzerCommand(const Caller::LinkInputs& in) {
  // Flags as before the cutover: the fuzzing and CmpLog binaries at -O2, the
  // witness (replay) binary without. CmpLog instruments the fuzzing program,
  // so it never carries the witness constructor.
  Caller::LinkInputs native = in;
  native.generator = NonDetGenerator::AFLPlusPlus;
  native.witness = in.witness && !in.cmplog;
  std::ostringstream command;
  if (in.cmplog) command << "env AFL_LLVM_CMPLOG=1 ";
  command << aflClangFastBinary() << " -g ";
  if (!native.witness) command << "-O2 ";
  command << "-fsanitize=fuzzer " << resultBitcode(native) << " ";
  if (native.witness) command << witnessObjectPath(native) << " ";
  command << kRustRuntimeLibrary << " " << kRustRuntimeSystemLibs << " -o "
          << fuzzerBinary(in);
  return command.str();
}

std::string buildReplayCommand(const std::string& hash,
                               const std::string& input) {
  std::string quoted;
  for (char c : input) {
    if (c == '\'')
      quoted += "'\\''";
    else
      quoted += c;
  }
  return "./" + hash + "-witness-fuzzed.out '" + quoted + "'";
}

bool prepareAflInputDir(const std::string& dir) {
  std::error_code error;
  std::filesystem::create_directories(dir, error);
  if (error) return false;
  const bool empty = std::filesystem::is_empty(dir, error);
  if (error) return false;
  if (!empty) return true;
  // afl-fuzz refuses an empty -i. The placeholder is as wide as the widest
  // value a generator reads, so CmpLog's input-to-state stage finds a whole
  // operand in the input from the first execution (D1 condition 4).
  std::ofstream seed(dir + "/seed", std::ios::binary);
  seed << std::string(kAflSeedMinBytes, 'A');
  return seed.good();
}

std::string installedKleeRuntime() {
  const char* prefix = getenv("MAP2CHECK_PATH");
  return std::string(prefix != nullptr ? prefix : "") +
         "/lib/map2check-klee.bc";
}

bool kleeRuntimeInstalled() {
  std::error_code error;
  return std::filesystem::is_regular_file(installedKleeRuntime(), error);
}

void warnBtreeIgnored() { Log::Warning(kBtreeIgnoredWarning); }

}  // namespace Map2Check
