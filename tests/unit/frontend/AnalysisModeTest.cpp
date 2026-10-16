/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

// The frontend's mode -> the -m2c-analysis-mode value it passes to opt, and
// from there the AnalysisMode number Map2CheckLibrary hands to
// map2check_set_mode. PassIRTest checks the same mapping through opt.

#include <gtest/gtest.h>

#include <map>
#include <string>

#include "../../../modules/frontend/caller.hpp"

using Map2Check::Map2CheckMode;
using Map2Check::runtimeAnalysisMode;

namespace {

// map2check-library's AnalysisMode, by -m2c-analysis-mode value.
int analysisModeNumber(const std::string& option) {
  static const std::map<std::string, int> numbers = {
      {"none", 0},   {"memtrack", 1}, {"memcleanup", 2},
      {"overflow", 3}, {"assert", 4}, {"target", 5}};
  auto it = numbers.find(option);
  return it == numbers.end() ? -1 : it->second;
}

int number(Map2CheckMode mode) {
  return analysisModeNumber(runtimeAnalysisMode(mode));
}

}  // namespace

TEST(AnalysisMode, EveryFrontendModeMapsToItsNumber) {
  EXPECT_EQ(number(Map2CheckMode::MEMTRACK_MODE), 1);
  EXPECT_EQ(number(Map2CheckMode::MEMCLEANUP_MODE), 2);
  EXPECT_EQ(number(Map2CheckMode::OVERFLOW_MODE), 3);
  EXPECT_EQ(number(Map2CheckMode::ASSERT_MODE), 4);
  EXPECT_EQ(number(Map2CheckMode::REACHABILITY_MODE), 5);
  EXPECT_EQ(number(Map2CheckMode::COVER_BRANCHES_MODE), 0);
}

TEST(AnalysisMode, OptionNames) {
  EXPECT_STREQ(runtimeAnalysisMode(Map2CheckMode::MEMTRACK_MODE), "memtrack");
  EXPECT_STREQ(runtimeAnalysisMode(Map2CheckMode::MEMCLEANUP_MODE), "memcleanup");
  EXPECT_STREQ(runtimeAnalysisMode(Map2CheckMode::OVERFLOW_MODE), "overflow");
  EXPECT_STREQ(runtimeAnalysisMode(Map2CheckMode::ASSERT_MODE), "assert");
  EXPECT_STREQ(runtimeAnalysisMode(Map2CheckMode::REACHABILITY_MODE), "target");
  EXPECT_STREQ(runtimeAnalysisMode(Map2CheckMode::COVER_BRANCHES_MODE), "none");
}

// No mode flag on the command line: map2check.cpp starts from kDefaultMode.
TEST(AnalysisMode, DefaultModeIsMemtrack) {
  EXPECT_EQ(Map2Check::kDefaultMode, Map2CheckMode::MEMTRACK_MODE);
  EXPECT_EQ(number(Map2Check::kDefaultMode), 1);
}
