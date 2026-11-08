# tacasv3a — Smart seeds plumbing: Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** make `--seed-exchange` actually move seeds between AFL++ and KLEE, across the hybrid's three phases.

**Architecture:**
- **Seed store.** A persistent directory `<cwd>/<programHash>.seeds/` next to the scratch directory, with `afl/`, `ktest/` and `replay/` subdirectories.
- **KLEE → AFL++.** KLEE vectors go to `afl/`, and the AFL++ phase uses `afl/` as its `-i`.
- **AFL++ → KLEE.** The AFL++ queue is replayed through the witness binary inside `replay/`, and the typed nondet log becomes one `.ktest` per entry in `ktest/`. KLEE then runs with `--seed-dir`.
- **Budget.** 0.2 / 0.6 / 0.2 when the exchange is on.
- **Cleanup.** The store is removed after the last phase unless `--debug`.
- **Where the code lives.** Pure selection logic goes into `modules/frontend/utils/seed_store.hpp`; the rest goes in `Caller` and `map2check.cpp`.

**Tech Stack:** C++17, GTest, bash integration tests, Docker dev image `map2check-dev:aflpp`. Use build dir `build_aflpp` (prefix pinned to `/workspace/build_aflpp/install`) and `build_aflpp_ut` (ENABLE_TEST). Rebuild `build_aflpp` only after R15 has finished, because it is using that install.

**Spec:** `docs/superpowers/specs/2026-09-28-tacasv3a-smart-seeds-plumbing-design.md`

## Global Constraints

- **Store path:** `<currentPath>/<programHash>.seeds`, absolute, with subdirectories `afl/`, `ktest/` and `replay/`. It exists only with `--seed-exchange`.
- **Queue replay:** at most 64 entries, in queue id order, each with a 2 s cap, run inside `replay/`. Empty or duplicate vectors are skipped.
- **KLEE seeding flags:** `--seed-dir=<store>/ktest --allow-seed-extension --allow-seed-truncation --seed-time=<max(1, kleeBudget/4)>s`, added only when `ktest/` has a file.
- **Budget with the exchange:** KLEE `min(0.6T, remaining−5)`. Without the exchange nothing changes (0.8T).
- **Log lines:** `Seeded KLEE with N vectors from AFL++` and `Seeded the fuzzer corpus with M vectors from KLEE`.
- **Default behaviour is unchanged** without `--seed-exchange`.
- Every mini-round is logged in `docs/reports/tacas-experiment-log.md`.

## Review Focus

- **A crash found by AFL++ must survive the queue replays.** Replays run in `replay/`, never in the phase directory. Pinned by integration test (2).
- **A run without `--debug` leaves no `*.seeds/` directory**, including when a phase errors out. Pinned by (3).
- **A queue entry whose replay hangs** is capped at 2 s. The `timeout` is covered by the code path; no dedicated test.
- **KLEE given seeds whose object sizes differ from its own** must not abort. The extension and truncation flags cover this, and it is exercised by (1).

---

### Task 1: `seed_store.hpp` (pure) + unit tests

**Files:**
- Create: `modules/frontend/utils/seed_store.hpp` and `tests/unit/frontend/SeedStoreTest.cpp`.
- Modify: `tests/unit/frontend/CMakeLists.txt`.

**Produces:**
- `std::string Map2Check::seedStorePath(const std::string& cwd, const std::string& programHash)` returns `cwd + "/" + programHash + ".seeds"`.
- `std::vector<std::string> Map2Check::selectQueueEntries(std::vector<std::string> names, size_t cap)` keeps only names starting with `id:`, sorts them lexicographically (which is id order, because AFL++ zero-pads), and truncates to `cap`.
- `bool Map2Check::isNewVector(const std::vector<uint8_t>& bytes, std::set<std::vector<uint8_t>>* seen)` returns false for an empty vector or one already in `seen`; otherwise it inserts the vector and returns true.

- [ ] **Step 1:** Write the tests below in `SeedStoreTest.cpp`, and register it in CMake the same way as `SlicerTest` (`add_executable(SeedStoreTest SeedStoreTest.cpp)` + `map2check_test(SeedStoreTest)`).

```cpp
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
```

- [ ] **Step 2:** Run `ninja SeedStoreTest` in `build_aflpp_ut`. Expected: FAIL, header not found.
- [ ] **Step 3:** Implement the header: `#include <algorithm> <cstdint> <set> <string> <vector>`, namespace `Map2Check`, the three inline functions exactly as specified, and a doc comment on each.
- [ ] **Step 4:** Run the test binary. Expected: `[  PASSED  ] 3 tests.`, and ctest 100%.
- [ ] **Step 5: Commit** `feat(tacasv3a): pure seed-store helpers`.

### Task 2: Store, KLEE → AFL++, budget and cleanup

**Files:** `modules/frontend/caller.hpp`, `modules/frontend/caller.cpp`, `modules/frontend/map2check.cpp`, `tests/integration/test_testcomp_regressions.sh` (section 11).

**Produces:**
- The member `std::string seedStore;` (absolute), set in the `Caller` constructor from `seedStorePath(currentPath, programHash)`.
- The accessor `const std::string& seedStorePath() const`.

- [ ] **Step 1: Failing tests.** In section 11 of the integration script:
  - Replace the `n_off` check with: after the run without the flag, `ls -d "$WORK/seed"/*.seeds 2>/dev/null | wc -l` must be 0.
  - Replace the `n_on` count with: after the `--seed-exchange --debug` run, count files in `"$WORK"/seed/*.seeds/afl/` whose names start with `klee-`. It must be > 0, and the log must contain `Seeded the fuzzer corpus with`.
  - Add a run of the same program with `--seed-exchange` **without** `--debug`. Afterwards `ls -d "$WORK/seed"/*.seeds` must be empty (`ok "no seed store is left behind without --debug"`).

  Run the suite. Expected: the `*.seeds` checks FAIL, because the store does not exist yet.
- [ ] **Step 2: Implement.**
  - Set `seedStore` in the constructor, right after `currentPath` is set.
  - `exportKleeVectorsAsSeeds` writes to `seedStore + "/afl"` (`create_directories`).
  - In the AFL++ branch, `inputDir` is `seedStore + "/afl"` when `seedExchange`, otherwise `"afl-in"`. The queue copy-back writes to `seedStore + "/afl/afl-" + name`.
  - KLEE budget: `const double kleeShare = this->seedExchange ? 0.6 : 0.8;` replaces the literal `0.8`.
  - Remove `exportFuzzerVectorAsKtest` and its `--seed-file` use. Task 3 replaces it.
  - In `map2check.cpp`, keep the store path from the last `map2check_execution`: a file-scope `std::string lastSeedStore` assigned from `caller->seedStorePath()`. After the hybrid phase sequence in `main`, when `args.seedExchange && !args.debugMode`, call `std::filesystem::remove_all(lastSeedStore, ec)`.
  - Remove `Caller::seedDirectory` if nothing else uses it.
- [ ] **Step 3:** Rebuild (after R15) and run the suite plus ctest. Expected: section 11 passes, and everything else is unchanged.
- [ ] **Step 4: Commit** `feat(tacasv3a): a seed store that survives the hybrid's phases`.

### Task 3: AFL++ → KLEE by replay, and KLEE `--seed-dir`

**Files:** `modules/frontend/caller.hpp`, `modules/frontend/caller.cpp`, `tests/integration/test_testcomp_regressions.sh`.

**Produces:** `unsigned Caller::exportFuzzerCorpusAsKtests();` (private). It returns the number of `.ktest` files written.

- [ ] **Step 1: Failing tests.** Add section 26, "seeds reach KLEE", and section 27, "a crash survives the replays":
  - **26:** `seed.c` from section 11, run with `--seed-exchange --debug`. The log must contain `Seeded KLEE with N vectors from AFL++` with N > 0 (grep the number), and the KLEE command line (debug) must contain `--seed-dir=`.
  - **27:** the program `int x = nondet(); if (x > 1000 && x < 1100) reach_error();` (AFL++ finds it quickly thanks to CmpLog/interesting values), run with `--seed-exchange --target-function --target-function-name reach_error --timeout 30`. It must report `VERIFICATION FAILED`.

  Run them. Expected: 26 FAILS (no such line); 27 passes (a regression guard; ledger it).
- [ ] **Step 2: Implement** `exportFuzzerCorpusAsKtests()`, called at the end of the AFL++ branch when `seedExchange && !isWitnessFileCreated()`:
  - List `afl-out/default/queue`, then `selectQueueEntries(names, 64)`.
  - For each entry:
    - clear `replay/` (`remove_all` + `create_directories`);
    - run `cd <replay> && timeout -k 1 2 <abs scratch>/<programHash>-witness-fuzzed.out < '<abs entry>' > /dev/null 2>&1`;
    - read `readNonDetLogAsObjects(<replay>/klee_log.csv)`;
    - skip it if the objects are empty or if `ktestToFuzzerBytes(objects)` is not new according to `isNewVector`;
    - otherwise `writeKtestFile(<store>/ktest/afl-<index>.ktest, objects)`.
  - Log `Seeded KLEE with N vectors from AFL++` when N > 0.
- [ ] **Step 3: KLEE flags.** In the KLEE branch, when `seedExchange` and `<store>/ktest` holds any regular file, append ` --seed-dir=<store>/ktest --allow-seed-extension --allow-seed-truncation --seed-time=<max(1, kleeBudget/4)>s` where `seedFlag` used to go (both command variants).
- [ ] **Step 4:** Rebuild and run the suite plus ctest. Expected: 26 and 27 pass, and everything else is unchanged.
- [ ] **Step 5: Commit** `feat(tacasv3a): the fuzzer corpus reaches KLEE as typed seeds`.

### Task 4: Mini-rounds

- [ ] **R16, Test-Comp:** the R15 manifests (`r15-ce.tsv`, `r15-cb.tsv`), 300 s, `EXTRA_FLAGS=--seed-exchange` (GENERATOR hybrid). Compare against the R15 control arm (same tasks, same code without the exchange). Report coverage, N and M per task (grep the `Seeded` lines from raw logs, or run a sample with `--debug`), and the median time.
- [ ] **R17, SV-COMP:** MemSafety and NoOverflows from the R14 manifests, plus a ReachSafety sample (`build_corpus.py` has no reachsafety property; use `--property cover-error`'s task list with `--target-function` via `run_testcomp_evaluation.sh`'s verdict column, or skip ReachSafety and note it). 120 s, with and without `--seed-exchange`. Report correct and wrong answers per arm.
- [ ] **Log R16–R17** with comparisons, and commit.
