/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include <gtest/gtest.h>
#include <stdlib.h>

#include <filesystem>
#include <fstream>
#include <string>

#include "../../../modules/frontend/utils/memory_budget.hpp"

namespace fs = std::filesystem;
using Map2Check::MemoryAction;

namespace {

class FakeCgroup : public ::testing::Test {
 protected:
  void SetUp() override {
    root = (fs::temp_directory_path() /
            ("m2c-cgroup-" + std::to_string(::getpid()) + "-" +
             ::testing::UnitTest::GetInstance()->current_test_info()->name()))
               .string();
    fs::remove_all(root);
    fs::create_directories(root);
    unsetenv("MAP2CHECK_MEMORY_LIMIT_MB");
  }
  void TearDown() override {
    fs::remove_all(root);
    unsetenv("MAP2CHECK_MEMORY_LIMIT_MB");
  }
  void write(const std::string& relative, const std::string& content) {
    const fs::path path = fs::path(root) / relative;
    fs::create_directories(path.parent_path());
    std::ofstream(path) << content;
  }
  std::string root;
};

}  // namespace

// BenchExec's container: the run's cgroup is the namespace root ("0::/").
TEST_F(FakeCgroup, ReadsTheLimitAtTheNamespaceRoot) {
  write("cgroup/memory.max", "2999996416\n");
  write("proc", "0::/\n");
  const std::string dir =
      Map2Check::limitingCgroup(root + "/cgroup", root + "/proc");
  EXPECT_EQ(dir, root + "/cgroup");
  EXPECT_EQ(Map2Check::runMemoryLimitMB(dir), 2861);
}

// A cgroup without a limit of its own ("max") inherits its parent's.
TEST_F(FakeCgroup, WalksUpToTheCgroupThatHasTheLimit) {
  write("cgroup/memory.max", "max\n");
  write("cgroup/run/memory.max", "3145728000\n");
  write("cgroup/run/tool/memory.max", "max\n");
  write("proc", "0::/run/tool\n");
  const std::string dir =
      Map2Check::limitingCgroup(root + "/cgroup", root + "/proc");
  EXPECT_EQ(dir, root + "/cgroup/run");
  EXPECT_EQ(Map2Check::runMemoryLimitMB(dir), 3000);
}

TEST_F(FakeCgroup, NoLimitAnywhereIsUnknown) {
  write("cgroup/memory.max", "max\n");
  write("proc", "0::/\n");
  const std::string dir =
      Map2Check::limitingCgroup(root + "/cgroup", root + "/proc");
  EXPECT_EQ(dir, "");
  EXPECT_EQ(Map2Check::runMemoryLimitMB(dir), 0);
}

TEST_F(FakeCgroup, NoCgroupFilesystemIsUnknown) {
  const std::string dir =
      Map2Check::limitingCgroup(root + "/absent", root + "/absent-proc");
  EXPECT_EQ(dir, "");
  EXPECT_EQ(Map2Check::runMemoryLimitMB(dir), 0);
}

TEST_F(FakeCgroup, TheEnvironmentOverridesTheCgroup) {
  write("cgroup/memory.max", "2999996416\n");
  setenv("MAP2CHECK_MEMORY_LIMIT_MB", "15000", 1);
  EXPECT_EQ(Map2Check::runMemoryLimitMB(root + "/cgroup"), 15000);
  setenv("MAP2CHECK_MEMORY_LIMIT_MB", "lots", 1);
  EXPECT_EQ(Map2Check::runMemoryLimitMB(root + "/cgroup"), 2861);
}

TEST(KleeMaxMemory, FortyPercentOfTheRunNeverAboveKleesDefault) {
  EXPECT_EQ(Map2Check::kleeMaxMemoryMB(3000), 1200);
  EXPECT_EQ(Map2Check::kleeMaxMemoryMB(15000), 2000);
  EXPECT_EQ(Map2Check::kleeMaxMemoryMB(400), 256);
  EXPECT_EQ(Map2Check::kleeMaxMemoryMB(0), 0);
}

TEST(NativeMemoryLimit, AThirdOfTheRun) {
  EXPECT_EQ(Map2Check::nativeMemoryLimitMB(3000), 1000);
  EXPECT_EQ(Map2Check::nativeMemoryLimitMB(15000), 5000);
  EXPECT_EQ(Map2Check::nativeMemoryLimitMB(300), 256);
  EXPECT_EQ(Map2Check::nativeMemoryLimitMB(0), 0);
}

TEST(MemoryAction, HaltsAtEightyAndKillsAtNinetyTwoPercent) {
  const long long mb = 1LL << 20;
  EXPECT_EQ(Map2Check::memoryAction(2000 * mb, 3000), MemoryAction::None);
  EXPECT_EQ(Map2Check::memoryAction(2400 * mb, 3000), MemoryAction::Halt);
  EXPECT_EQ(Map2Check::memoryAction(2760 * mb, 3000), MemoryAction::Kill);
  EXPECT_EQ(Map2Check::memoryAction(-1, 3000), MemoryAction::None);
  EXPECT_EQ(Map2Check::memoryAction(2900 * mb, 0), MemoryAction::None);
}
