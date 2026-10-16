/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

// Per-hook IR tests: compile a tiny C input with clang-16 -O0 -g -emit-llvm,
// run one pass plugin through opt-16, verify the module and check that the
// pass calls the core API of map2check-library (name, type, argument order).

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/raw_ostream.h>

#include <cstdlib>
#include <filesystem>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "../../../modules/backend/pass/RuntimeABI.hpp"
#include "../../../modules/frontend/utils/clang.hpp"

using Map2Check::runtimeCall;
using Map2Check::RuntimeHook;

namespace {

llvm::LLVMContext& context() {
  static llvm::LLVMContext ctx;
  return ctx;
}

std::string freshWorkDir() {
  static unsigned counter = 0;
  std::filesystem::path dir = std::filesystem::path(M2C_TEST_WORKDIR) /
                              ("run" + std::to_string(counter++));
  std::filesystem::remove_all(dir);
  std::filesystem::create_directories(dir);
  return dir.string();
}

int run(const std::string& command) {
  int rc = std::system(command.c_str());
  if (rc != 0) llvm::errs() << "command failed (" << rc << "): " << command << "\n";
  return rc;
}

/// Compiles inputs/<input> to bitcode as the tests do (-O0 -g, plus
/// `extraClangFlags`) and returns the path of the .bc file, or "" on error.
std::string compileInput(const std::string& dir, const std::string& input,
                         const std::string& extraClangFlags = "") {
  std::string bc = dir + "/input.bc";
  std::ostringstream cmd;
  cmd << M2C_TEST_CLANG << " -O0 -g -c -emit-llvm -Wno-everything "
      << extraClangFlags << " -o " << bc << " " << M2C_TEST_INPUTS << "/"
      << input << " 2> " << dir << "/clang.err";
  return run(cmd.str()) == 0 ? bc : "";
}

/// Runs plugin lib<plugin> with -passes='<pipeline>' (plus `optFlags`) on
/// inputs/<input> and parses the result.
std::unique_ptr<llvm::Module> runPassOn(const std::string& plugin,
                                        const std::string& pipeline,
                                        const std::string& input,
                                        const std::string& optFlags = "",
                                        const std::string& clangFlags = "") {
  std::string dir = freshWorkDir();
  std::string bc = compileInput(dir, input, clangFlags);
  if (bc.empty()) return nullptr;
  std::string out = dir + "/output.bc";
  std::ostringstream cmd;
  cmd << M2C_TEST_OPT << " -load-pass-plugin=" << M2C_PASS_PLUGIN_DIR << "/lib"
      << plugin << M2C_PASS_PLUGIN_SUFFIX << " " << optFlags << " -passes='"
      << pipeline << "' " << bc << " -o " << out << " 2> " << dir
      << "/opt.err";
  if (run(cmd.str()) != 0) return nullptr;
  llvm::SMDiagnostic err;
  std::unique_ptr<llvm::Module> m = llvm::parseIRFile(out, err, context());
  if (!m) err.print("PassIRTest", llvm::errs());
  return m;
}

std::vector<llvm::CallInst*> callsTo(llvm::Module& m, llvm::StringRef name) {
  std::vector<llvm::CallInst*> found;
  for (llvm::Function& f : m) {
    for (llvm::BasicBlock& bb : f) {
      for (llvm::Instruction& i : bb) {
        auto* call = llvm::dyn_cast<llvm::CallInst>(&i);
        if (call == nullptr) continue;
        auto* callee = llvm::dyn_cast<llvm::Function>(
            call->getCalledOperand()->stripPointerCasts());
        if (callee != nullptr && callee->getName() == name) {
          found.push_back(call);
        }
      }
    }
  }
  return found;
}

bool calls(llvm::Module& m, llvm::StringRef name) {
  return !callsTo(m, name).empty();
}

/// Position of the first call to `name` in function `fn`, or -1.
int callIndex(llvm::Module& m, llvm::StringRef name,
              llvm::StringRef fn = "__map2check_main__") {
  llvm::Function* f = m.getFunction(fn);
  if (f == nullptr) return -1;
  int index = 0;
  for (llvm::BasicBlock& bb : *f) {
    for (llvm::Instruction& i : bb) {
      if (auto* call = llvm::dyn_cast<llvm::CallInst>(&i)) {
        auto* callee = llvm::dyn_cast<llvm::Function>(
            call->getCalledOperand()->stripPointerCasts());
        if (callee != nullptr && callee->getName() == name) return index;
      }
      ++index;
    }
  }
  return -1;
}

/// The callee of every call to the hook is declared with the core type, and
/// each call passes arguments of exactly those types.
void expectCoreCalls(llvm::Module& m, RuntimeHook hook) {
  Map2Check::RuntimeCall core = runtimeCall(hook, m.getContext());
  std::vector<llvm::CallInst*> found = callsTo(m, core.name);
  ASSERT_FALSE(found.empty()) << "no call to " << core.name;
  llvm::Function* decl = m.getFunction(core.name);
  ASSERT_NE(decl, nullptr);
  EXPECT_EQ(decl->getFunctionType(), core.type) << core.name;
  for (llvm::CallInst* call : found) {
    EXPECT_EQ(call->getFunctionType(), core.type) << core.name;
    ASSERT_EQ(call->arg_size(), core.type->getNumParams()) << core.name;
    for (unsigned a = 0; a < call->arg_size(); ++a) {
      EXPECT_EQ(call->getArgOperand(a)->getType(), core.type->getParamType(a))
          << core.name << " argument " << a;
    }
  }
}

/// The string a `ptr` argument points to (a private global from
/// CreateGlobalStringPtr), or "" when it is not one.
std::string stringArg(llvm::Value* v) {
  auto* gv = llvm::dyn_cast<llvm::GlobalVariable>(v->stripPointerCasts());
  if (gv == nullptr || !gv->hasInitializer()) return "";
  auto* data = llvm::dyn_cast<llvm::ConstantDataSequential>(gv->getInitializer());
  if (data == nullptr || !data->isCString()) return "";
  return data->getAsCString().str();
}

int64_t intArg(llvm::Value* v) {
  auto* c = llvm::dyn_cast<llvm::ConstantInt>(v);
  return c == nullptr ? -1 : c->getSExtValue();
}

#define ASSERT_VALID(m)                                   \
  do {                                                    \
    ASSERT_NE((m), nullptr);                              \
    ASSERT_FALSE(llvm::verifyModule(*(m), &llvm::errs())); \
  } while (0)

}  // namespace

// ---------------------------------------------------------------------------
// TrackBasicBlockPass -> TrackBB
// ---------------------------------------------------------------------------

TEST(PassIR, TrackBasicBlockEmitsSaveBasicBlockLog) {
  auto m = runPassOn("TrackBasicBlockPass", "track-basic-block", "track_bb.c");
  ASSERT_VALID(m);
  expectCoreCalls(*m, RuntimeHook::TrackBB);
  EXPECT_FALSE(calls(*m, "map2check_track_bb"));
  EXPECT_EQ(m->getFunction("map2check_track_bb"), nullptr);
  for (llvm::CallInst* call : callsTo(*m, "map2check_save_basic_block_log")) {
    // (line, function name): a source line and the enclosing function.
    EXPECT_GT(intArg(call->getArgOperand(0)), 0);
    EXPECT_EQ(stringArg(call->getArgOperand(1)),
              call->getFunction()->getName().str());
  }
  // Both functions are tracked.
  bool inHelper = false;
  bool inMain = false;
  for (llvm::CallInst* call : callsTo(*m, "map2check_save_basic_block_log")) {
    inHelper |= call->getFunction()->getName() == "helper";
    inMain |= call->getFunction()->getName() == "main";
  }
  EXPECT_TRUE(inHelper);
  EXPECT_TRUE(inMain);
}

// ---------------------------------------------------------------------------
// AssertPass -> Assert
// ---------------------------------------------------------------------------

TEST(PassIR, AssertEmitsIsValidAssertWithConditionLast) {
  auto m = runPassOn("AssertPass", "assert-pass", "assert.c");
  ASSERT_VALID(m);
  expectCoreCalls(*m, RuntimeHook::Assert);
  EXPECT_FALSE(calls(*m, "map2check_assert"));
  EXPECT_EQ(m->getFunction("map2check_assert"), nullptr);
  std::vector<llvm::CallInst*> found = callsTo(*m, "map2check_is_valid_assert");
  ASSERT_EQ(found.size(), 1u);
  llvm::CallInst* call = found[0];
  // (line, function, expression): the condition moved from first to last.
  EXPECT_EQ(intArg(call->getArgOperand(0)), 5);
  EXPECT_EQ(stringArg(call->getArgOperand(1)), "main");
  std::vector<llvm::CallInst*> asserts = callsTo(*m, "__VERIFIER_assert");
  ASSERT_EQ(asserts.size(), 1u);
  EXPECT_EQ(call->getArgOperand(2), asserts[0]->getArgOperand(0));
}

TEST(PassIR, AssertFailEmitsIsValidAssertWithFalseCondition) {
  auto m = runPassOn("AssertPass", "assert-pass", "assert_fail.c");
  ASSERT_VALID(m);
  expectCoreCalls(*m, RuntimeHook::Assert);
  std::vector<llvm::CallInst*> found = callsTo(*m, "map2check_is_valid_assert");
  ASSERT_EQ(found.size(), 1u);
  std::vector<llvm::CallInst*> fails = callsTo(*m, "__assert_fail");
  ASSERT_EQ(fails.size(), 1u);
  // line and function come from the __assert_fail arguments; condition is 0.
  EXPECT_EQ(found[0]->getArgOperand(0), fails[0]->getArgOperand(2));
  EXPECT_EQ(found[0]->getArgOperand(1), fails[0]->getArgOperand(3));
  EXPECT_EQ(intArg(found[0]->getArgOperand(2)), 0);
}

// ---------------------------------------------------------------------------
// TargetPass -> TargetReached
// ---------------------------------------------------------------------------

TEST(PassIR, TargetEmitsReachTarget) {
  auto m = runPassOn("TargetPass", "target-pass", "target.c",
                     "-function-name=reach_error");
  ASSERT_VALID(m);
  expectCoreCalls(*m, RuntimeHook::TargetReached);
  EXPECT_FALSE(calls(*m, "map2check_target_function"));
  EXPECT_EQ(m->getFunction("map2check_target_function"), nullptr);
  std::vector<llvm::CallInst*> found = callsTo(*m, "map2check_reach_target");
  ASSERT_EQ(found.size(), 1u);
  // (function, scope, line), emitted right before the target call.
  EXPECT_EQ(stringArg(found[0]->getArgOperand(0)), "main");
  EXPECT_EQ(intArg(found[0]->getArgOperand(2)), 6);
  auto* next = llvm::dyn_cast<llvm::CallInst>(found[0]->getNextNode());
  ASSERT_NE(next, nullptr);
  EXPECT_EQ(next->getCalledOperand()->stripPointerCasts()->getName(),
            "reach_error");
}

// ---------------------------------------------------------------------------
// LoopPredAssumePass -> Assume
// ---------------------------------------------------------------------------

TEST(PassIR, LoopPredAssumeEmitsVerifierAssumeWithZext) {
  auto m = runPassOn("LoopPredAssumePass", "loop-pred-assume", "loop.c");
  ASSERT_VALID(m);
  expectCoreCalls(*m, RuntimeHook::Assume);
  EXPECT_FALSE(calls(*m, "map2check_assume_loop"));
  EXPECT_EQ(m->getFunction("map2check_assume_loop"), nullptr);
  std::vector<llvm::CallInst*> found = callsTo(*m, "__VERIFIER_assume");
  ASSERT_EQ(found.size(), 1u);
  // The i1 loop predicate is widened to the i32 the core takes.
  auto* zext = llvm::dyn_cast<llvm::ZExtInst>(found[0]->getArgOperand(0));
  ASSERT_NE(zext, nullptr);
  EXPECT_TRUE(zext->getSrcTy()->isIntegerTy(1));
  EXPECT_TRUE(llvm::isa<llvm::CmpInst>(zext->getOperand(0)));
}

// ---------------------------------------------------------------------------
// Map2CheckLibrary -> SetMode, Init, Success
// ---------------------------------------------------------------------------

struct ModeCase {
  const char* option;
  int value;
};

// Stable ctest names: print the option, not the bytes of the struct.
void PrintTo(const ModeCase& mode, std::ostream* os) { *os << mode.option; }

class LibraryMode : public ::testing::TestWithParam<ModeCase> {};

TEST_P(LibraryMode, EmitsSetModeBeforeInit) {
  const ModeCase& mode = GetParam();
  auto m = runPassOn("Map2CheckLibrary", "map2check-library", "main.c",
                     std::string("-m2c-analysis-mode=") + mode.option);
  ASSERT_VALID(m);
  expectCoreCalls(*m, RuntimeHook::SetMode);
  expectCoreCalls(*m, RuntimeHook::Init);
  int setMode = callIndex(*m, "map2check_set_mode");
  int init = callIndex(*m, "map2check_init");
  ASSERT_GE(setMode, 0);
  ASSERT_GE(init, 0);
  EXPECT_LT(setMode, init);
  std::vector<llvm::CallInst*> found = callsTo(*m, "map2check_set_mode");
  ASSERT_EQ(found.size(), 1u);
  EXPECT_EQ(intArg(found[0]->getArgOperand(0)), mode.value);
}

INSTANTIATE_TEST_SUITE_P(
    PassIR, LibraryMode,
    ::testing::Values(ModeCase{"none", 0}, ModeCase{"memtrack", 1},
                      ModeCase{"memcleanup", 2}, ModeCase{"overflow", 3},
                      ModeCase{"assert", 4}, ModeCase{"target", 5}),
    [](const ::testing::TestParamInfo<ModeCase>& info) {
      return std::string(info.param.option);
    });

TEST(PassIR, LibraryInitTakesNoArgumentAndSuccessPrecedesExits) {
  auto m = runPassOn("Map2CheckLibrary", "map2check-library", "main.c",
                     "-m2c-analysis-mode=memtrack");
  ASSERT_VALID(m);
  expectCoreCalls(*m, RuntimeHook::Init);
  expectCoreCalls(*m, RuntimeHook::Success);
  EXPECT_EQ(m->getFunction("main"), nullptr);
  ASSERT_NE(m->getFunction("__map2check_main__"), nullptr);
  // One success before exit(1), one before the return.
  EXPECT_EQ(callsTo(*m, "map2check_success").size(), 2u);
  for (llvm::CallInst* call : callsTo(*m, "exit")) {
    auto* prev = llvm::dyn_cast<llvm::CallInst>(call->getPrevNode());
    ASSERT_NE(prev, nullptr);
    EXPECT_EQ(prev->getCalledOperand()->getName(), "map2check_success");
  }
}

TEST(PassIR, LibraryDefaultModeIsNone) {
  auto m = runPassOn("Map2CheckLibrary", "map2check-library", "main.c");
  ASSERT_VALID(m);
  std::vector<llvm::CallInst*> found = callsTo(*m, "map2check_set_mode");
  ASSERT_EQ(found.size(), 1u);
  EXPECT_EQ(intArg(found[0]->getArgOperand(0)), 0);
}

// ---------------------------------------------------------------------------
// D1 condition 11: the analysed program keeps a defined unwind.
// ---------------------------------------------------------------------------

// Compiled with the flags the frontend uses for the program's bitcode, the
// renamed entry point is not nounwind, so the assume-rejection unwind of the
// library may cross it.
TEST(PassIR, ProgramBitcodeIsNotNounwind) {
  auto m = runPassOn("Map2CheckLibrary", "map2check-library", "assume.c",
                     "-m2c-analysis-mode=assert",
                     Map2Check::kProgramUnwindFlags);
  ASSERT_VALID(m);
  llvm::Function* entry = m->getFunction("__map2check_main__");
  ASSERT_NE(entry, nullptr);
  EXPECT_FALSE(entry->hasFnAttribute(llvm::Attribute::NoUnwind));
  llvm::Function* step = m->getFunction("step");
  ASSERT_NE(step, nullptr);
  EXPECT_FALSE(step->hasFnAttribute(llvm::Attribute::NoUnwind));
}

// Control: without the flag clang does mark the entry nounwind, so the test
// above measures the flag and not the input.
TEST(PassIR, ProgramBitcodeWithoutFlagIsNounwind) {
  auto m = runPassOn("Map2CheckLibrary", "map2check-library", "assume.c",
                     "-m2c-analysis-mode=assert");
  ASSERT_VALID(m);
  llvm::Function* entry = m->getFunction("__map2check_main__");
  ASSERT_NE(entry, nullptr);
  EXPECT_TRUE(entry->hasFnAttribute(llvm::Attribute::NoUnwind));
}

TEST(PassIR, ProgramUnwindFlagsNeverDisableUnwindTables) {
  std::string flags = Map2Check::kProgramUnwindFlags;
  EXPECT_NE(flags.find("-fexceptions"), std::string::npos);
  EXPECT_EQ(flags.find("-fno-unwind-tables"), std::string::npos);
  EXPECT_EQ(flags.find("-fno-asynchronous-unwind-tables"), std::string::npos);
}
