/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#ifndef MODULES_FRONTEND_UTILS_SEED_STORE_HPP_
#define MODULES_FRONTEND_UTILS_SEED_STORE_HPP_

#include <algorithm>
#include <cstdint>
#include <set>
#include <string>
#include <vector>

namespace Map2Check {

/** Where the engines leave seeds for each other under --seed-exchange.
 *
 * BESIDE the scratch directory, not inside it: every phase of the hybrid
 * builds a new Caller, which recreates the scratch directory empty, and a
 * store inside it never reached the next phase (tacasv3a spec, defect 1). */
inline std::string seedStorePath(const std::string& cwd,
                                 const std::string& programHash) {
  return cwd + "/" + programHash + ".seeds";
}

/** The AFL++ queue entries to hand to KLEE: only real entries ("id:..."),
 * in queue order -- AFL++ zero-pads the id, so lexicographic order is id
 * order, oldest (simplest) first -- and at most `cap` of them, so replaying
 * them cannot eat the phase. */
inline std::vector<std::string> selectQueueEntries(
    std::vector<std::string> names, size_t cap) {
  names.erase(std::remove_if(names.begin(), names.end(),
                             [](const std::string& name) {
                               return name.rfind("id:", 0) != 0;
                             }),
              names.end());
  std::sort(names.begin(), names.end());
  if (names.size() > cap) names.resize(cap);
  return names;
}

/** Whether a converted vector is worth a seed: not empty, and not one already
 * written -- many queue entries differ only in bytes the program never reads,
 * and they replay to the same typed vector. */
inline bool isNewVector(const std::vector<uint8_t>& bytes,
                        std::set<std::vector<uint8_t>>* seen) {
  if (bytes.empty()) return false;
  return seen->insert(bytes).second;
}

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_UTILS_SEED_STORE_HPP_
