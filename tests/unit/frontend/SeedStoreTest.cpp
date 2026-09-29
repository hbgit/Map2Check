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
  EXPECT_EQ(chosen[0], "id:000000,time:0,execs:0,orig:seed");
  EXPECT_EQ(chosen[1], "id:000001,src:000000,time:5");
}

TEST(IsNewVector, RejectsEmptyAndDuplicateVectors) {
  std::set<std::vector<uint8_t>> seen;
  EXPECT_FALSE(Map2Check::isNewVector({}, &seen));
  EXPECT_TRUE(Map2Check::isNewVector({1, 2}, &seen));
  EXPECT_FALSE(Map2Check::isNewVector({1, 2}, &seen));
  EXPECT_TRUE(Map2Check::isNewVector({1, 3}, &seen));
}
