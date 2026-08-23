/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

/**********************************************************************
 * Reading KLEE's own per-path test vectors.
 *
 * Cover-Error needs one test case, the violating one, and klee_log.csv holds
 * it. Cover-Branches needs many, one per explored path -- and KLEE already
 * computes exactly that: it writes klee-last/testNNNNNN.ktest per terminating
 * path, recording each symbolic object in the order the program consumed it.
 *
 * Taking them from KLEE rather than having the runtime write its own logs is
 * not a stylistic choice, it was measured. Making the instrumented program
 * write one file per state turned a 1-second run that answered FAILED into a
 * 100-second run that exhausted its budget and answered SUCCEEDED: every write
 * is an external call KLEE executes concretely, and enough of them corrupt the
 * result rather than merely slowing it. Reading .ktest files afterwards costs
 * the search nothing, because the search is already over.
 *
 * The format is read directly rather than by shelling out to ktest-tool: the
 * layout is nine lines of parsing, and a subprocess would put a Python
 * interpreter on the path between a verification run and its test suite.
 ***********************************************************************/

#ifndef MODULES_FRONTEND_TEST_SUITE_KTEST_READER_HPP_
#define MODULES_FRONTEND_TEST_SUITE_KTEST_READER_HPP_

#include <cstdint>
#include <string>
#include <vector>

namespace Map2Check {

/** One symbolic object: the name klee_make_symbolic was given, and its bytes.
 *
 * The name is where the type lives. A .ktest records name, size and bytes but
 * no type, so a four-byte object could be an int, a float or half of a long;
 * NonDetGeneratorKlee.c names them non_det_<type> precisely so this side can
 * tell. That naming was broken until recently -- every object was called
 * "non_det_#type", the macro's stringify operator having been written inside
 * the string literal -- which is why decoding by name is worth asserting. */
struct KtestObject {
  std::string name;
  std::vector<uint8_t> bytes;
};

/** Parses one .ktest file. Empty on a file that is absent or not a ktest. */
std::vector<KtestObject> readKtestFile(const std::string& path);

/** Renders one object as the decimal string a <input> element carries.
 *
 * Decoding is driven by the object's NAME, falling back to its size when the
 * name is unknown -- an unrecognised name is a new nondet type, not a reason
 * to drop a test case. */
std::string decodeKtestObject(const KtestObject& object);

/** The input vector of the path KLEE flagged as an error, if there is one.
 *
 * For Cover-Error, where the suite needs the ONE vector that reaches the bug.
 * The runtime's own klee_log.csv is supposed to hold it, and usually does not:
 * the state that reaches the target aborts, an aborted state runs no exit
 * handler, and the flush never happens. Measured on the Test-Comp corpus --
 * 290 of 376 runs that reported FAILED emitted a test case with zero <input>
 * elements, and TestCov scored every one of them as covering nothing.
 *
 * KLEE marks the offending path by writing testNNNNNN.<kind>.err beside its
 * .ktest, so the vector is identifiable without any cooperation from the
 * runtime. Returns an empty vector when no path errored. */
std::vector<std::string> readViolatingKtest(const std::string& kleeOutDir);

/** Every input vector KLEE recorded under `kleeOutDir`, one per .ktest.
 *
 * Ordered by file name, which is KLEE's own path numbering: stable across
 * runs, so two runs over the same output number their test cases alike.
 * `limit` bounds the suite. Vectors with no objects are dropped -- a path that
 * read no input is not a test case. */
std::vector<std::vector<std::string>> readKtestVectors(
    const std::string& kleeOutDir, size_t limit);

/** Serialises objects into the byte stream a fuzzer would consume.
 *
 * Sound only because both engines now agree on widths: NonDetGeneratorKlee.c
 * passes sizeof(type) to klee_make_symbolic, and NonDetGeneratorLibFuzzy.c
 * takes sizeof(type) bytes per read. Concatenating a .ktest's objects in order
 * therefore produces exactly the buffer that would drive the fuzzer down the
 * same path. Before the width fix this was impossible -- the fuzzer read one
 * byte per value whatever the type, so most vectors had nowhere to go. */
std::vector<uint8_t> ktestToFuzzerBytes(const std::vector<KtestObject>& objects);

/** Writes objects as a .ktest, for handing KLEE a starting point.
 *
 * The counterpart of readKtestFile, used to turn what one engine learned into
 * something the other can start from. False if the file could not be written. */
bool writeKtestFile(const std::string& path,
                    const std::vector<KtestObject>& objects);

/** Reads the runtime's nondet log as typed objects.
 *
 * The log is the only place a fuzzer run records what it actually consumed --
 * value AND type, in consumption order -- so it is the bridge from a fuzzer
 * result back into KLEE's format. Field 6 carries the NONDET_TYPE enumerator;
 * see modules/backend/library/header/Map2CheckTypes.h. */
std::vector<KtestObject> readNonDetLogAsObjects(const std::string& csvPath);

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_TEST_SUITE_KTEST_READER_HPP_
