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
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "../../../modules/backend/pass/RuntimeABI.hpp"
#include "../../../modules/frontend/caller.hpp"
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

/// Runs plugin lib<plugin> (or several, comma-separated) with -passes='<pipeline>' (plus `optFlags`) on
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
  cmd << M2C_TEST_OPT;
  // `plugin` may list several plugins, separated by commas.
  std::stringstream plugins(plugin);
  for (std::string one; std::getline(plugins, one, ',');) {
    cmd << " -load-pass-plugin=" << M2C_PASS_PLUGIN_DIR << "/lib" << one
        << M2C_PASS_PLUGIN_SUFFIX;
  }
  cmd << " " << optFlags << " -passes='"
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

// The option value the frontend passes for each of its modes reaches
// map2check_set_mode as the right AnalysisMode number.
struct FrontendModeCase {
  Map2Check::Map2CheckMode mode;
  const char* label;
  int value;
};

void PrintTo(const FrontendModeCase& c, std::ostream* os) { *os << c.label; }

class FrontendMode : public ::testing::TestWithParam<FrontendModeCase> {};

TEST_P(FrontendMode, SetModeGetsItsNumber) {
  const FrontendModeCase& c = GetParam();
  auto m = runPassOn("Map2CheckLibrary", "map2check-library", "main.c",
                     std::string("-m2c-analysis-mode=") +
                         Map2Check::runtimeAnalysisMode(c.mode));
  ASSERT_VALID(m);
  std::vector<llvm::CallInst*> found = callsTo(*m, "map2check_set_mode");
  ASSERT_EQ(found.size(), 1u);
  EXPECT_EQ(intArg(found[0]->getArgOperand(0)), c.value);
}

using Map2Check::Map2CheckMode;
INSTANTIATE_TEST_SUITE_P(
    PassIR, FrontendMode,
    ::testing::Values(
        FrontendModeCase{Map2CheckMode::MEMTRACK_MODE, "memtrack", 1},
        FrontendModeCase{Map2CheckMode::MEMCLEANUP_MODE, "memcleanup", 2},
        FrontendModeCase{Map2CheckMode::OVERFLOW_MODE, "overflow", 3},
        FrontendModeCase{Map2CheckMode::ASSERT_MODE, "assert", 4},
        FrontendModeCase{Map2CheckMode::REACHABILITY_MODE, "reachability", 5},
        FrontendModeCase{Map2CheckMode::COVER_BRANCHES_MODE, "cover_branches", 0},
        FrontendModeCase{Map2Check::kDefaultMode, "default", 1}),
    [](const ::testing::TestParamInfo<FrontendModeCase>& info) {
      return std::string(info.param.label);
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

// ---------------------------------------------------------------------------
// OverflowPass -> BinopAdd/Sub/Mul/SDiv/SRem
// ---------------------------------------------------------------------------

namespace {

/// The value a binop operand had before the pass widened it to i64.
llvm::Value* beforeSExt(llvm::Value* v) {
  if (auto* sext = llvm::dyn_cast<llvm::SExtInst>(v)) return sext->getOperand(0);
  return v;
}

/// Checks one binop call: it sits right before the operation it guards, its
/// operands are that operation's operands sign-extended to i64, and it passes
/// the operation's width, the line and the enclosing function.
void expectBinopCall(llvm::CallInst* call, unsigned opcode, int bits, int line) {
  auto* op = llvm::dyn_cast<llvm::BinaryOperator>(call->getNextNode());
  ASSERT_NE(op, nullptr) << "the check must precede the operation";
  EXPECT_EQ(op->getOpcode(), opcode);
  EXPECT_EQ(beforeSExt(call->getArgOperand(0)), op->getOperand(0));
  EXPECT_EQ(beforeSExt(call->getArgOperand(1)), op->getOperand(1));
  if (bits < 64) {
    EXPECT_TRUE(llvm::isa<llvm::SExtInst>(call->getArgOperand(0)));
    EXPECT_TRUE(llvm::isa<llvm::SExtInst>(call->getArgOperand(1)));
  }
  EXPECT_EQ(intArg(call->getArgOperand(2)), bits);
  if (line >= 0) EXPECT_EQ(intArg(call->getArgOperand(3)), line);
  EXPECT_EQ(stringArg(call->getArgOperand(4)),
            call->getFunction()->getName().str());
}

}  // namespace

TEST(PassIR, OverflowEmitsSignedBinopsWithBitsLineAndFunction) {
  auto m = runPassOn("OverflowPass", "overflow-pass", "overflow.c");
  ASSERT_VALID(m);
  expectCoreCalls(*m, RuntimeHook::BinopAdd);
  expectCoreCalls(*m, RuntimeHook::BinopSub);
  expectCoreCalls(*m, RuntimeHook::BinopMul);
  expectCoreCalls(*m, RuntimeHook::BinopSDiv);
  expectCoreCalls(*m, RuntimeHook::BinopSRem);

  // int a + b (line 6) and long + 1L (line 11); the unsigned add of line 12
  // has no nsw and is not checked.
  std::vector<llvm::CallInst*> add = callsTo(*m, "map2check_binop_add_signed");
  ASSERT_EQ(add.size(), 2u);
  expectBinopCall(add[0], llvm::Instruction::Add, 32, 6);
  expectBinopCall(add[1], llvm::Instruction::Add, 64, 11);

  std::vector<llvm::CallInst*> sub = callsTo(*m, "map2check_binop_sub_signed");
  ASSERT_EQ(sub.size(), 1u);
  expectBinopCall(sub[0], llvm::Instruction::Sub, 32, 7);
  std::vector<llvm::CallInst*> mul = callsTo(*m, "map2check_binop_mul_signed");
  ASSERT_EQ(mul.size(), 1u);
  expectBinopCall(mul[0], llvm::Instruction::Mul, 32, 8);
  std::vector<llvm::CallInst*> div = callsTo(*m, "map2check_binop_div_signed");
  ASSERT_EQ(div.size(), 1u);
  expectBinopCall(div[0], llvm::Instruction::SDiv, 32, 9);
  std::vector<llvm::CallInst*> rem = callsTo(*m, "map2check_binop_rem_signed");
  ASSERT_EQ(rem.size(), 1u);
  expectBinopCall(rem[0], llvm::Instruction::SRem, 32, 10);

  // No C name, no _uint variant (a C no-op), no dead overflowError.
  for (const char* legacy :
       {"map2check_binop_add", "map2check_binop_sub", "map2check_binop_mul",
        "map2check_binop_sdiv", "map2check_binop_srem",
        "map2check_binop_add_uint", "map2check_binop_sub_uint",
        "map2check_binop_mul_uint", "overflowError"}) {
    EXPECT_EQ(m->getFunction(legacy), nullptr) << legacy;
  }
}

TEST(PassIR, OverflowPassesTheOperationWidth) {
  auto m = runPassOn("OverflowPass", "overflow-pass", "overflow_widths.ll");
  ASSERT_VALID(m);
  struct WidthCase {
    const char* fn;
    const char* hook;
    unsigned opcode;
    int bits;
  };
  for (const WidthCase& c :
       {WidthCase{"add8", "map2check_binop_add_signed", llvm::Instruction::Add, 8},
        WidthCase{"sub16", "map2check_binop_sub_signed", llvm::Instruction::Sub, 16},
        WidthCase{"mul32", "map2check_binop_mul_signed", llvm::Instruction::Mul, 32},
        WidthCase{"add64", "map2check_binop_add_signed", llvm::Instruction::Add, 64}}) {
    SCOPED_TRACE(c.fn);
    std::vector<llvm::CallInst*> inFn;
    for (llvm::CallInst* call : callsTo(*m, c.hook)) {
      if (call->getFunction()->getName() == c.fn) inFn.push_back(call);
    }
    ASSERT_EQ(inFn.size(), 1u);
    expectBinopCall(inFn[0], c.opcode, c.bits, /*line=*/-1);
  }
}

// ---------------------------------------------------------------------------
// NonDetPass -> __VERIFIER_nondet_<t>, map2check_save_nondet_log_<t>, Assume
// ---------------------------------------------------------------------------

namespace {

struct NonDetCase {
  const char* cType;
  int line;  // of the call in inputs/nondet_types.c
};

void PrintTo(const NonDetCase& c, std::ostream* os) { *os << c.cType; }

int callLine(llvm::Instruction* i) {
  const llvm::DebugLoc& loc = i->getDebugLoc();
  return loc ? static_cast<int>(loc.getLine()) : -1;
}

/// The value stored in `slot` by a store that precedes `before` in its block.
llvm::Value* storedBefore(llvm::Value* slot, llvm::Instruction* before) {
  llvm::Value* stored = nullptr;
  for (llvm::Instruction& i : *before->getParent()) {
    if (&i == before) break;
    if (auto* store = llvm::dyn_cast<llvm::StoreInst>(&i)) {
      if (store->getPointerOperand() == slot) stored = store->getValueOperand();
    }
  }
  return stored;
}

llvm::Value* stripCasts(llvm::Value* v) {
  while (auto* cast = llvm::dyn_cast<llvm::CastInst>(v)) v = cast->getOperand(0);
  return v;
}

std::unique_ptr<llvm::Module>& nonDetModule() {
  static std::unique_ptr<llvm::Module> m =
      runPassOn("NonDetPass", "nondet-pass", "nondet_types.c");
  return m;
}

}  // namespace

class NonDetType : public ::testing::TestWithParam<NonDetCase> {};

// Each __VERIFIER_nondet_<t> call keeps (or gets) its core generator, and its
// value reaches map2check_save_nondet_log_<t> through an alloca of the C type
// with the NONDET_TYPE number as type_var.
TEST_P(NonDetType, LogsTheValueThroughATypedSlot) {
  const NonDetCase& c = GetParam();
  std::unique_ptr<llvm::Module>& m = nonDetModule();
  ASSERT_VALID(m);
  llvm::LLVMContext& ctx = m->getContext();
  Map2Check::NonDetValueSlot slot = Map2Check::nonDetValueSlot(c.cType, ctx);
  ASSERT_NE(slot.type, nullptr);

  // The generator call of this line.
  std::string generator = Map2Check::nonDetGenerator(c.cType);
  llvm::CallInst* gen = nullptr;
  for (llvm::CallInst* call : callsTo(*m, generator)) {
    if (callLine(call) == c.line) gen = call;
  }
  ASSERT_NE(gen, nullptr) << "no call to " << generator << " on line " << c.line;

  // Its logger: core name and type, (line, scope, type_var, slot, fn).
  std::string logger = Map2Check::nonDetLogger(c.cType);
  llvm::Function* decl = m->getFunction(logger);
  ASSERT_NE(decl, nullptr) << logger;
  EXPECT_EQ(decl->getFunctionType(), Map2Check::nonDetLoggerType(ctx));
  llvm::CallInst* log = nullptr;
  for (llvm::CallInst* call : callsTo(*m, logger)) {
    if (intArg(call->getArgOperand(0)) == c.line) log = call;
  }
  ASSERT_NE(log, nullptr) << "no " << logger << " for line " << c.line;
  EXPECT_EQ(log->getFunctionType(), Map2Check::nonDetLoggerType(ctx));
  ASSERT_EQ(log->arg_size(), 5u);
  EXPECT_TRUE(llvm::isa<llvm::ConstantInt>(log->getArgOperand(1)));  // scope
  EXPECT_EQ(intArg(log->getArgOperand(2)), slot.typeVar);
  auto* alloca = llvm::dyn_cast<llvm::AllocaInst>(log->getArgOperand(3));
  ASSERT_NE(alloca, nullptr) << "the value is passed through an alloca";
  EXPECT_EQ(alloca->getAllocatedType(), slot.type);
  EXPECT_EQ(stringArg(log->getArgOperand(4)), "main");
  // The slot holds the generator's value, converted to the C type.
  llvm::Value* stored = storedBefore(alloca, log);
  ASSERT_NE(stored, nullptr) << "nothing stored in the slot before the log";
  EXPECT_EQ(stored->getType(), slot.type);
  EXPECT_EQ(stripCasts(stored), gen);
  // The log follows the generator call in the same block.
  EXPECT_EQ(gen->getParent(), log->getParent());
  EXPECT_TRUE(gen->comesBefore(log));
}

INSTANTIATE_TEST_SUITE_P(
    PassIR, NonDetType,
    ::testing::Values(NonDetCase{"int", 18}, NonDetCase{"unsigned", 19},
                      NonDetCase{"uint", 20}, NonDetCase{"char", 21},
                      NonDetCase{"uchar", 22}, NonDetCase{"short", 23},
                      NonDetCase{"ushort", 24}, NonDetCase{"long", 25},
                      NonDetCase{"ulong", 26}, NonDetCase{"bool", 27},
                      NonDetCase{"pointer", 28}, NonDetCase{"pchar", 29},
                      NonDetCase{"size_t", 30}, NonDetCase{"loff_t", 31},
                      NonDetCase{"sector_t", 32}, NonDetCase{"double", 33}),
    [](const ::testing::TestParamInfo<NonDetCase>& info) {
      return std::string(info.param.cType);
    });

TEST(PassIR, NonDetEmitsNoCRuntimeName) {
  std::unique_ptr<llvm::Module>& m = nonDetModule();
  ASSERT_VALID(m);
  for (llvm::Function& f : *m) {
    EXPECT_FALSE(f.getName().starts_with("map2check_non_det_")) << f.getName().str();
    EXPECT_FALSE(f.getName().starts_with("map2check_nondet_")) << f.getName().str();
  }
  // The library has no __VERIFIER_nondet_unsigned: that call now goes to
  // __VERIFIER_nondet_uint and is still logged as unsigned.
  llvm::Function* unsignedDecl = m->getFunction("__VERIFIER_nondet_unsigned");
  if (unsignedDecl != nullptr) EXPECT_TRUE(unsignedDecl->use_empty());
  EXPECT_EQ(callsTo(*m, "__VERIFIER_nondet_uint").size(), 2u);
  EXPECT_EQ(callsTo(*m, "map2check_save_nondet_log_unsigned").size(), 1u);
  EXPECT_EQ(callsTo(*m, "map2check_save_nondet_log_uint").size(), 1u);
}

namespace {

void expectNoNounwind(llvm::Module& m, llvm::StringRef name) {
  llvm::Function* f = m.getFunction(name);
  ASSERT_NE(f, nullptr) << name.str();
  EXPECT_FALSE(f->hasFnAttribute(llvm::Attribute::NoUnwind)) << name.str();
  std::vector<llvm::CallInst*> found = callsTo(m, name);
  EXPECT_FALSE(found.empty()) << name.str();
  for (llvm::CallInst* call : found) {
    EXPECT_FALSE(call->hasFnAttr(llvm::Attribute::NoUnwind))
        << "nounwind call to " << name.str() << " in "
        << call->getFunction()->getName().str();
  }
}

}  // namespace

// A nounwind declaration of __VERIFIER_assume (and calls to it): the assume
// of the library unwinds, so the pass clears nounwind on both.
TEST(PassIR, NonDetKeepsVerifierAssumeAndDropsNounwind) {
  auto m = runPassOn("NonDetPass", "nondet-pass", "loop_assume.ll");
  ASSERT_VALID(m);
  expectCoreCalls(*m, RuntimeHook::Assume);
  EXPECT_EQ(m->getFunction("map2check_assume"), nullptr);
  expectNoNounwind(*m, "__VERIFIER_assume");
}

TEST(PassIR, NonDetTurnsCrabAssumeIntoVerifierAssume) {
  auto m = runPassOn("NonDetPass", "nondet-pass", "crab_assume.ll");
  ASSERT_VALID(m);
  expectCoreCalls(*m, RuntimeHook::Assume);
  EXPECT_FALSE(calls(*m, "verifier.assume"));
  EXPECT_EQ(m->getFunction("map2check_crab_assume"), nullptr);
  std::vector<llvm::CallInst*> found = callsTo(*m, "__VERIFIER_assume");
  ASSERT_EQ(found.size(), 2u);
  for (llvm::CallInst* call : found) {
    // The i1 invariant is widened to the i32 the core takes.
    auto* zext = llvm::dyn_cast<llvm::ZExtInst>(call->getArgOperand(0));
    ASSERT_NE(zext, nullptr);
    EXPECT_TRUE(zext->getSrcTy()->isIntegerTy(1));
    EXPECT_TRUE(llvm::isa<llvm::ICmpInst>(zext->getOperand(0)));
  }
  expectNoNounwind(*m, "__VERIFIER_assume");
}

// assume_abort_if_not aborts on a false condition; its body becomes a call to
// the library's assume. A program's own __VERIFIER_assume loses its body, so
// the library's definition is the only one (no recursion, no duplicate).
TEST(PassIR, NonDetRewritesAbortingAssumes) {
  auto m = runPassOn("NonDetPass", "nondet-pass", "assume_defined.c");
  ASSERT_VALID(m);
  llvm::Function* verifierAssume = m->getFunction("__VERIFIER_assume");
  ASSERT_NE(verifierAssume, nullptr);
  EXPECT_TRUE(verifierAssume->isDeclaration());
  EXPECT_EQ(verifierAssume->getFunctionType(),
            runtimeCall(RuntimeHook::Assume, m->getContext()).type);

  llvm::Function* abortIfNot = m->getFunction("assume_abort_if_not");
  ASSERT_NE(abortIfNot, nullptr);
  ASSERT_FALSE(abortIfNot->isDeclaration());
  std::vector<llvm::CallInst*> inBody;
  for (llvm::CallInst* call : callsTo(*m, "__VERIFIER_assume")) {
    if (call->getFunction() == abortIfNot) inBody.push_back(call);
  }
  ASSERT_EQ(inBody.size(), 1u);
  EXPECT_FALSE(calls(*m, "abort"));
  // cond != 0, as C reads the int condition.
  auto* zext = llvm::dyn_cast<llvm::ZExtInst>(inBody[0]->getArgOperand(0));
  ASSERT_NE(zext, nullptr);
  auto* cmp = llvm::dyn_cast<llvm::ICmpInst>(zext->getOperand(0));
  ASSERT_NE(cmp, nullptr);
  EXPECT_EQ(cmp->getPredicate(), llvm::ICmpInst::ICMP_NE);
  EXPECT_EQ(cmp->getOperand(0), abortIfNot->getArg(0));

  expectNoNounwind(*m, "__VERIFIER_assume");
  expectNoNounwind(*m, "assume_abort_if_not");
}

// The loop predicate goes to the program's own __VERIFIER_assume declaration
// when there is one; without -fexceptions that declaration and its calls are
// nounwind, and the library's assume unwinds through them.
TEST(PassIR, LoopPredAssumeDropsNounwind) {
  auto m = runPassOn("LoopPredAssumePass", "loop-pred-assume", "loop_assume.ll");
  ASSERT_VALID(m);
  expectCoreCalls(*m, RuntimeHook::Assume);
  EXPECT_EQ(callsTo(*m, "__VERIFIER_assume").size(), 2u);
  expectNoNounwind(*m, "__VERIFIER_assume");
}

// ---------------------------------------------------------------------------
// MemoryTrackPass -> Malloc, Calloc, Alloca, NonStaticAlloca, StorePointer,
// Load, CheckFree, MapFree, FreeResolved, PosixMemalign, Wasm*
// ---------------------------------------------------------------------------

namespace {

std::unique_ptr<llvm::Module>& memTrackModule() {
  static std::unique_ptr<llvm::Module> m =
      runPassOn("MemoryTrackPass", "memory-track", "memtrack.c");
  return m;
}

/// Calls to `name` whose argument `lineArg` is `line`.
std::vector<llvm::CallInst*> callsOnLine(llvm::Module& m, llvm::StringRef name,
                                         unsigned lineArg, int line) {
  std::vector<llvm::CallInst*> found;
  for (llvm::CallInst* call : callsTo(m, name)) {
    if (intArg(call->getArgOperand(lineArg)) == line) found.push_back(call);
  }
  return found;
}

/// The program's call to `name` on source line `line`.
llvm::CallInst* programCall(llvm::Module& m, llvm::StringRef name, int line) {
  for (llvm::CallInst* call : callsTo(m, name)) {
    if (callLine(call) == line) return call;
  }
  return nullptr;
}

}  // namespace

TEST(PassIR, MemoryTrackEmitsOnlyCoreNames) {
  std::unique_ptr<llvm::Module>& m = memTrackModule();
  ASSERT_VALID(m);
  for (RuntimeHook hook :
       {RuntimeHook::Malloc, RuntimeHook::Calloc, RuntimeHook::Alloca,
        RuntimeHook::NonStaticAlloca, RuntimeHook::StorePointer,
        RuntimeHook::Load, RuntimeHook::CheckFree, RuntimeHook::MapFree,
        RuntimeHook::FreeResolved, RuntimeHook::PosixMemalign}) {
    SCOPED_TRACE(runtimeCall(hook, m->getContext()).name);
    expectCoreCalls(*m, hook);
  }
  for (const char* legacy :
       {"map2check_malloc", "map2check_calloc", "map2check_free",
        "map2check_load", "map2check_check_deref", "map2check_alloca",
        "map2check_non_static_alloca", "map2check_add_store_pointer",
        "map2check_free_resolved_address", "map2check_posix",
        "map2check_function"}) {
    EXPECT_EQ(m->getFunction(legacy), nullptr) << legacy;
  }
}

// free(p) of a pointer variable: load *slot, skip NULL, then check_free and
// map_free, both (name, slot, *slot, scope, line, fn), before the free.
TEST(PassIR, MemoryTrackFreeOfAVariableChecksThenMapsTheLoadedBlock) {
  std::unique_ptr<llvm::Module>& m = memTrackModule();
  ASSERT_VALID(m);
  EXPECT_EQ(callsTo(*m, "map2check_check_free").size(), 3u);
  EXPECT_EQ(callsTo(*m, "map2check_map_free").size(), 3u);
  for (int line : {15, 17, 18}) {
    SCOPED_TRACE(line);
    llvm::CallInst* freeCall = programCall(*m, "free", line);
    ASSERT_NE(freeCall, nullptr);
    auto* programLoad = llvm::dyn_cast<llvm::LoadInst>(freeCall->getArgOperand(0));
    ASSERT_NE(programLoad, nullptr);
    llvm::Value* slot = programLoad->getPointerOperand();

    std::vector<llvm::CallInst*> check =
        callsOnLine(*m, "map2check_check_free", 4, line);
    std::vector<llvm::CallInst*> map =
        callsOnLine(*m, "map2check_map_free", 4, line);
    ASSERT_EQ(check.size(), 1u);
    ASSERT_EQ(map.size(), 1u);
    // Same arguments, check first, in the same guarded block.
    for (unsigned a = 0; a < 6; ++a) {
      EXPECT_EQ(check[0]->getArgOperand(a), map[0]->getArgOperand(a)) << a;
    }
    ASSERT_EQ(check[0]->getParent(), map[0]->getParent());
    EXPECT_TRUE(check[0]->comesBefore(map[0]));
    EXPECT_EQ(check[0]->getArgOperand(1), slot);
    auto* loaded = llvm::dyn_cast<llvm::LoadInst>(check[0]->getArgOperand(2));
    ASSERT_NE(loaded, nullptr) << "the block address is *slot";
    EXPECT_EQ(loaded->getPointerOperand(), slot);
    EXPECT_TRUE(loaded->getType()->isPointerTy());
    EXPECT_TRUE(llvm::isa<llvm::ConstantInt>(check[0]->getArgOperand(3)));
    EXPECT_EQ(stringArg(check[0]->getArgOperand(5)), "main");

    // The guard: the block runs only when *slot is not NULL, and both paths
    // reach the free.
    llvm::BasicBlock* guarded = check[0]->getParent();
    llvm::BasicBlock* pred = guarded->getSinglePredecessor();
    ASSERT_NE(pred, nullptr);
    auto* br = llvm::dyn_cast<llvm::BranchInst>(pred->getTerminator());
    ASSERT_NE(br, nullptr);
    ASSERT_TRUE(br->isConditional());
    auto* cmp = llvm::dyn_cast<llvm::ICmpInst>(br->getCondition());
    ASSERT_NE(cmp, nullptr);
    EXPECT_EQ(cmp->getOperand(0), loaded);
    EXPECT_TRUE(llvm::isa<llvm::ConstantPointerNull>(cmp->getOperand(1)));
    llvm::BasicBlock* nonNull =
        cmp->getPredicate() == llvm::ICmpInst::ICMP_NE ? br->getSuccessor(0)
                                                        : br->getSuccessor(1);
    llvm::BasicBlock* isNull =
        nonNull == br->getSuccessor(0) ? br->getSuccessor(1) : br->getSuccessor(0);
    EXPECT_EQ(nonNull, guarded);
    EXPECT_EQ(isNull, freeCall->getParent());
    EXPECT_EQ(guarded->getSingleSuccessor(), freeCall->getParent());
  }
}

// free(make()) frees an address with no variable: one resolved-address check
// with is_null_valid 0. realloc(p, n) checks its old block with
// is_null_valid 1, then maps the new block as a malloc.
TEST(PassIR, MemoryTrackFreeOfAnAddressAndReallocUseFreeResolved) {
  std::unique_ptr<llvm::Module>& m = memTrackModule();
  ASSERT_VALID(m);
  std::vector<llvm::CallInst*> resolved =
      callsTo(*m, "map2check_check_free_resolved_address");
  ASSERT_EQ(resolved.size(), 2u);

  llvm::CallInst* freeCall = programCall(*m, "free", 16);
  ASSERT_NE(freeCall, nullptr);
  std::vector<llvm::CallInst*> onFree =
      callsOnLine(*m, "map2check_check_free_resolved_address", 1, 16);
  ASSERT_EQ(onFree.size(), 1u);
  EXPECT_EQ(onFree[0]->getArgOperand(0), freeCall->getArgOperand(0));
  EXPECT_EQ(stringArg(onFree[0]->getArgOperand(2)), "main");
  EXPECT_TRUE(onFree[0]->getArgOperand(3)->getType()->isIntegerTy(16));
  EXPECT_EQ(intArg(onFree[0]->getArgOperand(3)), 0);
  EXPECT_TRUE(onFree[0]->comesBefore(freeCall));

  llvm::CallInst* reallocCall = programCall(*m, "realloc", 8);
  ASSERT_NE(reallocCall, nullptr);
  std::vector<llvm::CallInst*> onRealloc =
      callsOnLine(*m, "map2check_check_free_resolved_address", 1, 8);
  ASSERT_EQ(onRealloc.size(), 1u);
  EXPECT_EQ(onRealloc[0]->getArgOperand(0), reallocCall->getArgOperand(0));
  EXPECT_EQ(intArg(onRealloc[0]->getArgOperand(3)), 1);
  EXPECT_TRUE(onRealloc[0]->comesBefore(reallocCall));
  bool mapped = false;
  for (llvm::CallInst* call : callsTo(*m, "map2check_map_malloc")) {
    if (call->getArgOperand(0) == reallocCall) {
      mapped = true;
      EXPECT_EQ(intArg(call->getArgOperand(1)), 32);
    }
  }
  EXPECT_TRUE(mapped);
}

TEST(PassIR, MemoryTrackMapsMallocCallocAndPosixMemalign) {
  std::unique_ptr<llvm::Module>& m = memTrackModule();
  ASSERT_VALID(m);
  // (block, i32 size): malloc in make() and main().
  for (int line : {3, 6}) {
    llvm::CallInst* mallocCall = programCall(*m, "malloc", line);
    ASSERT_NE(mallocCall, nullptr) << line;
    bool mapped = false;
    for (llvm::CallInst* call : callsTo(*m, "map2check_map_malloc")) {
      if (call->getArgOperand(0) != mallocCall) continue;
      mapped = true;
      EXPECT_EQ(intArg(call->getArgOperand(1)), line == 3 ? 4 : 16);
      EXPECT_TRUE(mallocCall->comesBefore(call));
    }
    EXPECT_TRUE(mapped) << line;
  }
  // (block, i32 qty, i32 size)
  llvm::CallInst* callocCall = programCall(*m, "calloc", 7);
  ASSERT_NE(callocCall, nullptr);
  std::vector<llvm::CallInst*> calloc = callsTo(*m, "map2check_map_calloc");
  ASSERT_EQ(calloc.size(), 1u);
  EXPECT_EQ(calloc[0]->getArgOperand(0), callocCall);
  EXPECT_EQ(intArg(calloc[0]->getArgOperand(1)), 2);
  EXPECT_EQ(intArg(calloc[0]->getArgOperand(2)), 4);
  // (memptr, i32 size)
  llvm::CallInst* posix = programCall(*m, "posix_memalign", 12);
  ASSERT_NE(posix, nullptr);
  std::vector<llvm::CallInst*> mapped = callsTo(*m, "map2check_map_posix_memalign");
  ASSERT_EQ(mapped.size(), 1u);
  EXPECT_EQ(mapped[0]->getArgOperand(0), posix->getArgOperand(0));
  EXPECT_EQ(intArg(mapped[0]->getArgOperand(1)), 64);
  EXPECT_TRUE(posix->comesBefore(mapped[0]));
}

// map_alloca(name, addr, i32 size, i32 size_primitive, i32 line, i32 scope)
// for the static array and the global; map_non_static_alloca takes the VLA's
// element count.
TEST(PassIR, MemoryTrackMapsAllocasGlobalsAndVlas) {
  std::unique_ptr<llvm::Module>& m = memTrackModule();
  ASSERT_VALID(m);
  bool array = false;
  bool global = false;
  for (llvm::CallInst* call : callsTo(*m, "map2check_map_alloca")) {
    llvm::Value* addr = call->getArgOperand(1);
    if (auto* alloca = llvm::dyn_cast<llvm::AllocaInst>(addr)) {
      if (alloca->getAllocatedType()->isArrayTy()) {
        array = true;
        EXPECT_EQ(intArg(call->getArgOperand(2)), 20);
        EXPECT_EQ(intArg(call->getArgOperand(3)), 4);
        EXPECT_TRUE(llvm::isa<llvm::ConstantInt>(call->getArgOperand(4)));
        EXPECT_TRUE(llvm::isa<llvm::ConstantInt>(call->getArgOperand(5)));
      }
    } else if (addr == m->getNamedGlobal("g")) {
      global = true;
      EXPECT_EQ(stringArg(call->getArgOperand(0)), "g");
      EXPECT_EQ(intArg(call->getArgOperand(2)), 4);
      EXPECT_EQ(intArg(call->getArgOperand(3)), 4);
    }
  }
  EXPECT_TRUE(array);
  EXPECT_TRUE(global);

  std::vector<llvm::CallInst*> vla = callsTo(*m, "map2check_map_non_static_alloca");
  ASSERT_EQ(vla.size(), 1u);
  auto* alloca = llvm::dyn_cast<llvm::AllocaInst>(vla[0]->getArgOperand(1));
  ASSERT_NE(alloca, nullptr);
  EXPECT_TRUE(alloca->isArrayAllocation());
  // The element count, narrowed to i32; the element size.
  EXPECT_EQ(stripCasts(vla[0]->getArgOperand(2)),
            stripCasts(alloca->getArraySize()));
  EXPECT_EQ(intArg(vla[0]->getArgOperand(3)), 4);
  EXPECT_EQ(intArg(vla[0]->getArgOperand(4)), 10);
}

// check_load(ptr, i32 line, i32 scope, i32 size, fn) before each load and
// store; map_store_pointer(var, value, scope, name, line, fn) after a store
// of a pointer.
TEST(PassIR, MemoryTrackChecksLoadsAndStoresAndMapsStoredPointers) {
  std::unique_ptr<llvm::Module>& m = memTrackModule();
  ASSERT_VALID(m);
  // arr[0] = *q on line 13: the load of q (8 bytes), of *q and the store (4).
  std::vector<llvm::CallInst*> line13 = callsOnLine(*m, "map2check_check_load", 1, 13);
  ASSERT_GE(line13.size(), 3u);
  std::multiset<int64_t> sizes;
  for (llvm::CallInst* call : line13) {
    sizes.insert(intArg(call->getArgOperand(3)));
    EXPECT_TRUE(llvm::isa<llvm::ConstantInt>(call->getArgOperand(2)));
    EXPECT_EQ(stringArg(call->getArgOperand(4)), "main");
    // Right before the access it checks.
    llvm::Instruction* next = call->getNextNode();
    ASSERT_TRUE(llvm::isa<llvm::LoadInst>(next) || llvm::isa<llvm::StoreInst>(next));
    llvm::Value* accessed = llvm::isa<llvm::LoadInst>(next)
                                ? llvm::cast<llvm::LoadInst>(next)->getPointerOperand()
                                : llvm::cast<llvm::StoreInst>(next)->getPointerOperand();
    EXPECT_EQ(call->getArgOperand(0)->stripPointerCasts(),
              accessed->stripPointerCasts());
  }
  EXPECT_GE(sizes.count(8), 1u);
  EXPECT_GE(sizes.count(4), 2u);

  // p = malloc(...) on line 6.
  std::vector<llvm::CallInst*> stored = callsOnLine(*m, "map2check_map_store_pointer", 4, 6);
  ASSERT_EQ(stored.size(), 1u);
  auto* store = llvm::dyn_cast<llvm::StoreInst>(stored[0]->getPrevNode());
  ASSERT_NE(store, nullptr);
  EXPECT_EQ(stored[0]->getArgOperand(0), store->getPointerOperand());
  EXPECT_EQ(stored[0]->getArgOperand(1), store->getValueOperand());
  EXPECT_TRUE(llvm::isa<llvm::ConstantInt>(stored[0]->getArgOperand(2)));
  EXPECT_EQ(stringArg(stored[0]->getArgOperand(5)), "main");
}

// The nondet value slots NonDetPass adds are not program memory.
TEST(PassIR, MemoryTrackSkipsTheNonDetSlots) {
  auto m = runPassOn("NonDetPass,MemoryTrackPass", "nondet-pass,memory-track",
                     "nondet_types.c");
  ASSERT_VALID(m);
  EXPECT_TRUE(calls(*m, "map2check_check_load"));
  for (const char* hook : {"map2check_check_load", "map2check_map_alloca",
                           "map2check_map_store_pointer"}) {
    for (llvm::CallInst* call : callsTo(*m, hook)) {
      llvm::Value* addr =
          call->getArgOperand(llvm::StringRef(hook) == "map2check_map_alloca" ? 1 : 0);
      auto* alloca = llvm::dyn_cast<llvm::AllocaInst>(addr->stripPointerCasts());
      if (alloca == nullptr) continue;
      EXPECT_FALSE(alloca->hasMetadata(Map2Check::kRuntimeMetadata))
          << hook << " on " << alloca->getName().str();
    }
  }
}

TEST(PassIR, MemoryTrackWasmModeEmitsWasmHooks) {
  auto m = runPassOn("MemoryTrackPass", "memory-track", "wasm.ll", "-wasm-mode");
  ASSERT_VALID(m);
  expectCoreCalls(*m, RuntimeHook::WasmMalloc);
  expectCoreCalls(*m, RuntimeHook::WasmFree);
  expectCoreCalls(*m, RuntimeHook::WasmCheckAccess);
  llvm::CallInst* alloc = programCall(*m, "w2c_m_dlmalloc", -1);
  ASSERT_NE(alloc, nullptr);
  std::vector<llvm::CallInst*> malloc = callsTo(*m, "map2check_wasm_malloc");
  ASSERT_EQ(malloc.size(), 1u);
  EXPECT_EQ(stripCasts(malloc[0]->getArgOperand(0)), alloc);
  EXPECT_EQ(intArg(malloc[0]->getArgOperand(1)), 16);  // zext of the i32 size
  std::vector<llvm::CallInst*> access = callsTo(*m, "map2check_wasm_check_access");
  ASSERT_EQ(access.size(), 2u);  // the store and the load
  for (llvm::CallInst* call : access) EXPECT_EQ(intArg(call->getArgOperand(1)), 4);
  std::vector<llvm::CallInst*> freed = callsTo(*m, "map2check_wasm_free");
  ASSERT_EQ(freed.size(), 1u);
  EXPECT_EQ(stripCasts(freed[0]->getArgOperand(0)), alloc);
  EXPECT_TRUE(freed[0]->getArgOperand(1)->getType()->isIntegerTy(64));
}
