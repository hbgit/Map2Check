/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include <gtest/gtest.h>

#include "../../../modules/frontend/utils/alternation.hpp"

using Map2Check::Engine;

TEST(AlternationWindow, StartsAtTheBaseShareOfTheBudget) {
  EXPECT_DOUBLE_EQ(Map2Check::alternationWindow(Engine::Fuzzer, 1, 300, 300),
                   30);
  EXPECT_DOUBLE_EQ(Map2Check::alternationWindow(Engine::Klee, 1, 300, 270), 90);
}

TEST(AlternationWindow, DoublesEveryRound) {
  EXPECT_DOUBLE_EQ(Map2Check::alternationWindow(Engine::Fuzzer, 2, 300, 300),
                   60);
  EXPECT_DOUBLE_EQ(Map2Check::alternationWindow(Engine::Klee, 3, 300, 1000),
                   360);
}

TEST(AlternationWindow, NeverExceedsWhatIsLeft) {
  EXPECT_DOUBLE_EQ(Map2Check::alternationWindow(Engine::Klee, 2, 300, 40), 40);
}

TEST(StagnationSeconds, IsFivePercentButAtLeastTen) {
  EXPECT_EQ(Map2Check::stagnationSeconds(300), 15u);
  EXPECT_EQ(Map2Check::stagnationSeconds(60), 10u);
}

TEST(CoverageWatch, StagnatesOnlyAfterTheQuietPeriod) {
  Map2Check::CoverageWatch watch(10);
  EXPECT_FALSE(watch.stagnated(100, 0));
  EXPECT_FALSE(watch.stagnated(100, 9));
  EXPECT_FALSE(watch.stagnated(120, 9.5));  // progress resets the clock
  EXPECT_FALSE(watch.stagnated(120, 19));
  EXPECT_TRUE(watch.stagnated(120, 19.5));
}

TEST(CoverageWatch, AnUnreadableSampleIsNotEvidence) {
  Map2Check::CoverageWatch watch(10);
  EXPECT_FALSE(watch.stagnated(-1, 0));
  EXPECT_FALSE(watch.stagnated(-1, 50));  // never read: no verdict on progress
  EXPECT_FALSE(watch.stagnated(5, 51));
  EXPECT_FALSE(watch.stagnated(-1, 60));
  EXPECT_TRUE(watch.stagnated(5, 61.5));
}

TEST(StagnationSeconds, KleesPatienceDoublesWithItsRounds) {
  EXPECT_EQ(Map2Check::stagnationSeconds(300, 1), 15u);
  EXPECT_EQ(Map2Check::stagnationSeconds(300, 2), 30u);
  EXPECT_EQ(Map2Check::stagnationSeconds(300, 3), 60u);
}
