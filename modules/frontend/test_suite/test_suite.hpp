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
 * klee_log.csv on exit, under both the KLEE and the LibFuzzer generator. This
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

  /** Writes testcase-<n>.xml, numbering from 1 in call order. */
  bool writeTestCase(const std::vector<std::string>& inputs, bool coversError);

 private:
  std::string directory;
  unsigned counter;
};

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_TEST_SUITE_TEST_SUITE_HPP_
