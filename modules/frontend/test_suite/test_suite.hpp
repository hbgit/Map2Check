/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

/**********************************************************************
 * Emission of Test-Comp test suites.
 *
 * The format is defined by https://gitlab.com/sosy-lab/software/test-format:
 * a directory holding one metadata.xml plus one *.xml per test case. Each test
 * case is a sequence of <input> elements whose order is the order in which the
 * program consumed nondeterministic values.
 *
 * Map2Check already produces that sequence. The runtime appends every
 * __VERIFIER_nondet_* call to an ordered log (NonDetLog.c) and flushes it to
 * klee_log.csv on exit, under both the KLEE and the AFL++ generator. This
 * module only serializes it -- no new instrumentation is involved, and the
 * emitter is therefore engine-agnostic by construction.
 *
 * Scope: one test case per run. map2check_exit() guards on a static
 * alreadyReleased flag, so the log is flushed once, for the violating
 * execution. That covers Cover-Error. Cover-Branches needs a suite of many
 * test cases and therefore per-input logs, which is separate work.
 ***********************************************************************/

#ifndef MODULES_FRONTEND_TEST_SUITE_TEST_SUITE_HPP_
#define MODULES_FRONTEND_TEST_SUITE_TEST_SUITE_HPP_

#include <filesystem>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace Map2Check {

/** Fields of metadata.xml. The DTD declares them as a sequence, not a choice,
 * so the writer emits them in exactly this order. */
struct TestSuiteMetadata {
  std::string producer;
  std::string specification;
  std::string programFile;
  std::string programHash;
  std::string entryFunction;
  std::string architecture;
  std::string creationTime;
};

/** Current UTC time as "YYYY-MM-DDThh:mm:ssZ". */
std::string isoUtcNow();

/** Reads the runtime's nondet log and returns the recorded values in
 * consumption order.
 *
 * Malformed rows are skipped rather than fatal: a log truncated by a crash or
 * a budget kill still yields a usable prefix, and a partial test case is worth
 * more than none. Returns an empty vector when the file is absent. */
std::vector<std::string> readNonDetLog(const std::string& csvPath);

/** Writes a Test-Comp test suite into a directory, creating it if needed. */
class TestSuiteWriter {
 public:
  explicit TestSuiteWriter(std::string directory);

  /** Writes metadata.xml. False if the file could not be written. */
  bool writeMetadata(const TestSuiteMetadata& metadata);

  /** Writes testcase-<n>.xml, numbering after the cases already in the
   * directory (an earlier phase of the same run may have written some).
   *
   * `pruned` marks the case as a path a failed assumption ended (an XML
   * comment, which TestCov ignores), so a later phase can evict it. */
  bool writeTestCase(const std::vector<std::string>& inputs, bool coversError,
                     bool pruned = false);

  /** How many test cases the directory holds, written earlier or by this. */
  size_t caseCount() const { return cases.size(); }
  /** How many of them are assumption-pruned paths. */
  size_t prunedCount() const { return prunedCases.size(); }
  /** Removes one assumption-pruned case to make room for a deeper one.
   *
   * The suite is written phase by phase under a cap. On xcsp/AllInterval the
   * first KLEE phase found only pruned paths and filled the cap with them, and
   * the deep paths of the next phase found no room: 95% coverage fell to 9%.
   * False when there is no pruned case to remove. */
  bool evictPrunedCase();
  /** Whether a test case with exactly these inputs is already there. */
  bool hasTestCase(const std::vector<std::string>& inputs) const;

  /** Removes every testcase-<n>.xml from `directory` -- run once, at the start
   * of a run, so a suite never mixes cases from an earlier run. */
  static void removeTestCases(const std::string& directory);

 private:
  std::string directory;
  unsigned counter;
  /** The inputs of every case in the directory, XML-escaped as written. */
  std::set<std::vector<std::string>> cases;
  /** The pruned cases among them, with the file each one lives in. */
  std::map<std::vector<std::string>, std::filesystem::path> prunedCases;
};

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_TEST_SUITE_TEST_SUITE_HPP_
