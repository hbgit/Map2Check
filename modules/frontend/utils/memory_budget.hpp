/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#ifndef MODULES_FRONTEND_UTILS_MEMORY_BUDGET_HPP_
#define MODULES_FRONTEND_UTILS_MEMORY_BUDGET_HPP_

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <string>

namespace Map2Check {

/** The memory a run may use, and how much of it the engines get.
 *
 * BenchExec enforces its limit on the run's cgroup as a whole: the engines,
 * the native replays and the tmpfs that holds every file the tool writes
 * ("a RAM disk that is included in the memory limit"). Crossing it ends the
 * whole run as OUT OF MEMORY, with no test suite -- even when what crossed it
 * was one throwaway process. The Test-Comp 2027 pre-run (3 GB) lost 1,747
 * runs that way. None of the engines knows the limit: KLEE's own cap is a
 * fixed 2,000 MB that its seeding loop never checks, and afl-fuzz runs its
 * target with no memory limit at all.
 *
 * Inside BenchExec's container the run's cgroup is the root of the cgroup
 * namespace, so /sys/fs/cgroup/memory.max is the limit and memory.current
 * what is used, tmpfs included (measured with runexec 3.34). */

/** Bytes in `path` (a cgroup v2 memory.max or memory.current), or -1 when it
 * is missing, unreadable or "max" (no limit). */
inline long long readCgroupBytes(const std::string& path) {
  std::ifstream file(path);
  std::string value;
  if (!(file >> value) || value.empty() ||
      value.find_first_not_of("0123456789") != std::string::npos) {
    return -1;
  }
  return std::stoll(value);
}

/** The directory of this process's cgroup v2 under `root`, from a
 * /proc/self/cgroup listing ("0::/path"); `root` itself when the listing has
 * no v2 entry. */
inline std::string cgroupDirectory(const std::string& root,
                                   const std::string& procCgroup) {
  std::ifstream file(procCgroup);
  std::string line;
  while (std::getline(file, line)) {
    if (line.rfind("0::", 0) == 0) {
      std::string path = line.substr(3);
      if (path == "/") path.clear();
      return root + path;
    }
  }
  return root;
}

/** Where the run's limit is enforced: the closest cgroup, from this process's
 * own up to `root`, whose memory.max holds a number. Empty when none does. */
inline std::string limitingCgroup(
    const std::string& root = "/sys/fs/cgroup",
    const std::string& procCgroup = "/proc/self/cgroup") {
  std::string dir = cgroupDirectory(root, procCgroup);
  while (true) {
    if (readCgroupBytes(dir + "/memory.max") > 0) return dir;
    if (dir.size() <= root.size()) return "";
    dir = dir.substr(0, dir.rfind('/'));
  }
}

/** The run's memory limit in MB, or 0 when it is unknown.
 * MAP2CHECK_MEMORY_LIMIT_MB, when set to a number, wins over the cgroup. */
inline long long runMemoryLimitMB(const std::string& cgroup) {
  if (const char* forced = std::getenv("MAP2CHECK_MEMORY_LIMIT_MB")) {
    const std::string value(forced);
    if (!value.empty() &&
        value.find_first_not_of("0123456789") == std::string::npos) {
      return std::stoll(value);
    }
  }
  if (cgroup.empty()) return 0;
  const long long bytes = readCgroupBytes(cgroup + "/memory.max");
  return bytes > 0 ? bytes >> 20 : 0;
}

/** KLEE's --max-memory for a run limited to `limitMB`: 40% of it, never above
 * KLEE's own default of 2,000 MB -- so a 15 GB run keeps exactly the cap it was
 * measured with, and a 3 GB one gets 1,200 MB. 0 means "leave KLEE's
 * default". The other 60% is what the tmpfs (seed store, KLEE's output, the
 * fuzzer's queue), the frontend and the replays have needed: on the
 * Intel-TDX tasks the files alone reached 1 GB. */
inline long long kleeMaxMemoryMB(long long limitMB) {
  if (limitMB <= 0) return 0;
  return std::max(256LL, std::min(2000LL, limitMB * 2 / 5));
}

/** The address-space limit, in MB, for a program run natively: afl-fuzz's
 * target (-m) and the replays of vectors. A third of the run's limit, at
 * least 256 MB; 0 when the limit is unknown (no limit, as before). A target
 * that allocates past it fails its malloc, as it would on a smaller machine,
 * instead of taking the run down with it. */
inline long long nativeMemoryLimitMB(long long limitMB) {
  if (limitMB <= 0) return 0;
  return std::max(256LL, limitMB / 3);
}

/** What the KLEE watchdog does on a reading of the run's memory. */
enum class MemoryAction { None, Halt, Kill };

/** Halt KLEE (SIGINT: it stops and writes its tests) at 80% of the limit;
 * kill it at 92%, where writing the tests of the live states could itself
 * cross the limit. Both apply to the whole run, because that is what
 * BenchExec measures. */
inline MemoryAction memoryAction(long long usedBytes, long long limitMB) {
  if (usedBytes < 0 || limitMB <= 0) return MemoryAction::None;
  const long long limit = limitMB << 20;
  if (usedBytes >= limit / 100 * 92) return MemoryAction::Kill;
  if (usedBytes >= limit / 100 * 80) return MemoryAction::Halt;
  return MemoryAction::None;
}

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_UTILS_MEMORY_BUDGET_HPP_
