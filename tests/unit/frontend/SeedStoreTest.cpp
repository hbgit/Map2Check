/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include <gtest/gtest.h>

#include <set>
#include <string>
#include <vector>

#include "../../../modules/frontend/utils/seed_store.hpp"

// Next to the scratch directory, not inside it: every hybrid phase recreates
// the scratch directory, and a store inside it never reached the next phase.
TEST(SeedStorePath, SitsBesideTheScratchDirectory) {
  EXPECT_EQ(Map2Check::seedStorePath("/work", "abc.map2check"),
            "/work/abc.map2check.seeds");
}

TEST(SelectQueueEntries, KeepsOnlyQueueEntriesInIdOrderUpToTheCap) {
  const std::vector<std::string> names = {
      "id:000002,src:000000,time:9", ".state", "README.txt",
      "id:000000,time:0,execs:0,orig:seed", "id:000001,src:000000,time:5"};
  const std::vector<std::string> chosen =
      Map2Check::selectQueueEntries(names, 2);
  ASSERT_EQ(chosen.size(), 2u);
  // The ",orig:" seed is not the fuzzer's discovery (see the test below).
  EXPECT_EQ(chosen[0], "id:000001,src:000000,time:5");
  EXPECT_EQ(chosen[1], "id:000002,src:000000,time:9");
}

TEST(IsNewVector, RejectsEmptyAndDuplicateVectors) {
  std::set<std::vector<uint8_t>> seen;
  EXPECT_FALSE(Map2Check::isNewVector({}, &seen));
  EXPECT_TRUE(Map2Check::isNewVector({1, 2}, &seen));
  EXPECT_FALSE(Map2Check::isNewVector({1, 2}, &seen));
  EXPECT_TRUE(Map2Check::isNewVector({1, 3}, &seen));
}

// The fuzzer's own seeds come back in its queue tagged ",orig:" -- the KLEE
// vectors, the previous rounds' entries -- and KLEE already has those. Only
// what this round discovered is worth replaying (tacas 3b).
TEST(SelectQueueEntries, SkipsTheSeedsTheFuzzerStartedFrom) {
  const auto picked = Map2Check::selectQueueEntries(
      {"id:000000,time:0,execs:0,orig:klee-0", "id:000001,src:000000,op:havoc"},
      8);
  ASSERT_EQ(picked.size(), 1u);
  EXPECT_EQ(picked[0], "id:000001,src:000000,op:havoc");
}
