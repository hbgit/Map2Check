/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * Map2Check -> GPL-2.0
 * CLANG     -> Apache-2.0
 * KLEE      -> NCSA
 * CRAB-LLVM -> Apache-2.0
 * STP,Z3 -> MIT
 *
 * SPDX-License-Identifier: (GPL-2.0 AND Apache-2.0 AND NCSA AND MIT)
 **/

#include "caller.hpp"

#include <stdlib.h>
// CPP Libs
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

#include <atomic>
#include <thread>

#ifdef MAP2CHECK_HAVE_SQLITE
#include <sqlite3.h>
#endif

#include "test_suite/ktest_reader.hpp"
#include "test_suite/test_suite.hpp"
#include "utils/gen_crypto_hash.hpp"
#include "utils/alternation.hpp"
#include "utils/log.hpp"
#include "utils/seed_store.hpp"
#include "utils/slicer.hpp"
#include "utils/tools.hpp"
// namespace fs = boost::filesystem;
// }  // namespace

using std::ifstream;
using std::regex;
using std::smatch;

namespace {
inline std::string getLibSuffix() { return ".so"; }

bool isWitnessFileCreated() {
  Map2Check::Log::Debug("Checking file");
  std::ifstream infile("map2check_checked_error");
  if (infile.is_open()) {
    Map2Check::Log::Debug("Found file!");
    return true;
  }
  return false;
}
}  // namespace

namespace Map2Check {
Caller::Caller(std::string bc_program_path, Map2CheckMode mode,
               NonDetGenerator generator) {
  // this->cleanGarbage();
  this->pathprogram = bc_program_path;
  this->map2checkMode = mode;
  this->nonDetGenerator = generator;
  GenHash hash;
  hash.setFilePath(bc_program_path);
  if (hash.generate_sha1_hash_for_file() != 0) {
    throw std::runtime_error("cannot read the input program " +
                             bc_program_path);
  }
  this->programHash = hash.getOutputSha1HashFile() + ".map2check";

  // The scratch directory is named after the SHA-1 of the input bitcode, so it
  // is content-derived and not run-derived: analysing the same input twice
  // resolves to the same name. A plain mkdir over an existing directory fails
  // silently and leaves whatever a previous, possibly aborted, run left behind
  // -- including map2check_property. Now that a recorded violation survives a
  // budget expiry, a stale property file would be read as a real result and
  // fabricate a FALSE. Start from an empty directory.
  std::ostringstream createTempDir;
  createTempDir.str("");
  createTempDir << "rm -rf " << programHash << " && mkdir " << programHash;
  system(createTempDir.str().c_str());

  std::ostringstream moveProgram;
  moveProgram << "cp " << bc_program_path << " " << programHash;
  system(moveProgram.str().c_str());

  Map2Check::Log::Debug("Changing current dir");
  currentPath = std::filesystem::current_path().string();
  seedStore = Map2Check::seedStorePath(currentPath, programHash);
  sliceCache = Map2Check::sliceCachePath(currentPath, programHash);
  buildCache = currentPath + "/" + programHash + ".build";
  std::filesystem::current_path(currentPath + "/" + programHash);
  Map2Check::Log::Debug("Current path: " +
                        std::filesystem::current_path().string());
}

namespace {
/** Wall-clock start of the PROCESS, not of this Caller.
 *
 * The hybrid rebuilds the Caller once per engine, so a per-object start time
 * would reset the clock at every phase and defeat the whole point. Function
 * static: initialised on the first call, which happens before any engine runs.
 */
std::chrono::steady_clock::time_point processStart() {
  static const std::chrono::steady_clock::time_point start =
      std::chrono::steady_clock::now();
  return start;
}
}  // namespace

unsigned Caller::remainingOf(unsigned timeout) {
  const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                           std::chrono::steady_clock::now() - processStart())
                           .count();
  const long long left = static_cast<long long>(timeout) - elapsed;
  return left < 1 ? 1u : static_cast<unsigned>(left);
}

unsigned Caller::remainingSeconds() const {
  const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                           std::chrono::steady_clock::now() - processStart())
                           .count();
  if (elapsed < 0) return this->timeout;
  const long long left = static_cast<long long>(this->timeout) - elapsed;
  // Never zero. A phase given no time at all is a phase that cannot even
  // report that it had none, and the caller has no way to tell that apart
  // from a crash.
  return left < 1 ? 1u : static_cast<unsigned>(left);
}

std::string Caller::preOptimizationFlags() {
  std::ostringstream flags;
  flags.str("");
  flags << "-O0";
  return flags.str();
}

std::string Caller::postOptimizationFlags() {
  std::ostringstream flags;
  flags.str("");
  flags << "-O2 ";
  return flags.str();
}

namespace {
/** KLEE's covered-instruction count, from the last row of the SQLite stats
 * database it rewrites every second; -1 when it cannot be read (not created
 * yet, busy, or a build without SQLite). */
long long kleeCoveredInstructions(const std::string &statsPath) {
#ifdef MAP2CHECK_HAVE_SQLITE
  sqlite3 *db = nullptr;
  long long covered = -1;
  if (sqlite3_open_v2(statsPath.c_str(), &db, SQLITE_OPEN_READONLY, nullptr) ==
      SQLITE_OK) {
    sqlite3_busy_timeout(db, 200);
    sqlite3_stmt *statement = nullptr;
    if (sqlite3_prepare_v2(db,
                           "SELECT CoveredInstructions FROM stats ORDER BY "
                           "rowid DESC LIMIT 1",
                           -1, &statement, nullptr) == SQLITE_OK &&
        sqlite3_step(statement) == SQLITE_ROW) {
      covered = sqlite3_column_int64(statement, 0);
    }
    sqlite3_finalize(statement);
  }
  sqlite3_close(db);
  return covered;
#else
  (void)statsPath;
  return -1;
#endif
}
}  // namespace

int Caller::runKleeWatched(const std::string &command) {
  this->stoppedOnStagnation = false;
#ifndef MAP2CHECK_HAVE_SQLITE
  return system(command.c_str());
#else
  if (this->stagnationLimit == 0) return system(command.c_str());

  // The shell's pid becomes the pid of `timeout` through exec, and `timeout`
  // forwards a SIGINT to KLEE -- whose handler halts the search cleanly and
  // writes the tests of the states it finished.
  std::error_code error;
  std::filesystem::remove("klee.pid", error);
  const std::string wrapped = "echo $$ > klee.pid; exec " + command;
  std::atomic<bool> done{false};
  int result = 0;
  std::thread runner([&wrapped, &done, &result] {
    result = system(wrapped.c_str());
    done = true;
  });

  Map2Check::CoverageWatch watch(this->stagnationLimit);
  const auto started = std::chrono::steady_clock::now();
  const std::string stats = std::string(Map2Check::kleeOutputDir) + "/run.stats";
  while (!done) {
    for (int tick = 0; tick < 4 && !done; ++tick) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    if (done) break;
    const double now = std::chrono::duration<double>(
                           std::chrono::steady_clock::now() - started)
                           .count();
    if (!watch.stagnated(kleeCoveredInstructions(stats), now)) continue;

    // To KLEE itself, the child of `timeout`: signalled, `timeout` forwards
    // to the child AND its process group, KLEE gets two SIGINTs, and the
    // second one exits before the halt dump of the live states is written.
    pid_t pid = 0;
    std::ifstream("klee.pid") >> pid;
    pid_t klee = 0;
    if (pid > 0) {
      std::ifstream children("/proc/" + std::to_string(pid) + "/task/" +
                             std::to_string(pid) + "/children");
      children >> klee;
    }
    if (klee > 0) {
      kill(klee, SIGINT);
    } else if (pid > 0) {
      kill(pid, SIGINT);
    }
    this->stoppedOnStagnation = true;
    Map2Check::Log::Warning("KLEE stagnated: no new coverage for " +
                            std::to_string(this->stagnationLimit) +
                            " s -- stopping it");
    break;
  }
  runner.join();
  return result;
#endif
}

bool Caller::replayKleeVectorsForViolation() {
  // KLEE's partial paths -- the states it dumped at a halt -- stop where the
  // search stopped. Run natively, completed with zeros past their end (the
  // AFL++ generator's rule), some of them go on to the error: that is how the
  // fixed hybrid covered eca-* tasks KLEE left UNKNOWN, by accident, in its
  // last fuzzer phase's dry run. Done on purpose here, right after KLEE, and
  // without needing --seed-exchange.
  std::error_code error;
  std::string witness;
  for (const auto &entry :
       std::filesystem::directory_iterator(buildCache, error)) {
    const std::string candidate =
        entry.path().string() + "/" + programHash + "-witness-fuzzed.out";
    if (std::filesystem::exists(candidate, error)) witness = candidate;
  }
  if (witness.empty()) return false;

  std::vector<std::string> tests;
  for (const auto &entry : std::filesystem::directory_iterator(
           Map2Check::kleeOutputDir, error)) {
    if (entry.path().extension() == ".ktest") {
      tests.push_back(entry.path().string());
    }
  }
  std::sort(tests.begin(), tests.end());

  const std::string replay =
      std::filesystem::absolute("vector-replay", error).string();
  const auto started = std::chrono::steady_clock::now();
  const double allowedSeconds = std::min(
      0.1 * static_cast<double>(this->timeout),
      static_cast<double>(this->remainingSeconds()) - 5.0);
  if (allowedSeconds < 1.0) return false;
  const auto allowed = std::chrono::duration<double>(allowedSeconds);
  std::set<std::vector<uint8_t>> seen;
  unsigned replayed = 0;
  for (const std::string &test : tests) {
    if (std::chrono::steady_clock::now() - started >= allowed) break;
    const std::vector<uint8_t> bytes =
        Map2Check::ktestToFuzzerBytes(Map2Check::readKtestFile(test));
    if (!seen.insert(bytes).second) continue;
    std::filesystem::remove_all(replay, error);
    std::filesystem::create_directories(replay, error);
    {
      std::ofstream input(replay + "/input", std::ios::binary);
      input.write(reinterpret_cast<const char *>(bytes.data()),
                  static_cast<std::streamsize>(bytes.size()));
    }
    std::ostringstream command;
    command << "cd '" << replay << "' && timeout -k 1 2 '" << witness
            << "' < input > /dev/null 2>&1";
    system(command.str().c_str());
    ++replayed;
    if (!std::filesystem::exists(replay + "/map2check_checked_error", error)) {
      continue;
    }
    // A violation: its files (property, nondet log, trace) become this
    // phase's, as a confirmed fuzzer crash's do.
    for (const auto &entry :
         std::filesystem::directory_iterator(replay, error)) {
      if (!entry.is_regular_file(error)) continue;
      if (entry.path().filename() == "input") continue;
      std::filesystem::copy_file(
          entry.path(), entry.path().filename(),
          std::filesystem::copy_options::overwrite_existing, error);
    }
    std::filesystem::remove_all(replay, error);
    Map2Check::Log::Info("A KLEE vector completed with zeros reaches the "
                         "violation (" + std::to_string(replayed) +
                         " replayed)");
    return true;
  }
  std::filesystem::remove_all(replay, error);
  return false;
}

void Caller::keepKleeTestsAsSeeds() {
  // The latest tests, not the first: KLEE numbers them as states finish, and
  // the later ones are the deeper paths the next turn should start from.
  std::error_code error;
  const std::string kept = seedStore + "/kleeprev";
  std::filesystem::remove_all(kept, error);
  std::vector<std::string> tests;
  for (const auto &entry : std::filesystem::directory_iterator(
           Map2Check::kleeOutputDir, error)) {
    if (entry.path().extension() == ".ktest") {
      tests.push_back(entry.path().string());
    }
  }
  if (tests.empty()) return;
  std::sort(tests.begin(), tests.end());
  if (tests.size() > kMaxSeedsFromFuzzer) {
    tests.erase(tests.begin(), tests.end() - kMaxSeedsFromFuzzer);
  }
  std::filesystem::create_directories(kept, error);
  for (const std::string &test : tests) {
    std::filesystem::copy_file(
        test, kept + "/" + std::filesystem::path(test).filename().string(),
        std::filesystem::copy_options::overwrite_existing, error);
  }
}

unsigned Caller::exportKleeVectorsAsSeeds() {
  std::error_code error;
  const std::string aflSeeds = seedStore + "/afl";
  std::filesystem::create_directories(aflSeeds, error);

  std::vector<std::string> tests;
  for (const auto &entry : std::filesystem::directory_iterator(
           Map2Check::kleeOutputDir, error)) {
    if (entry.path().extension() == ".ktest") {
      tests.push_back(entry.path().string());
    }
  }
  // All of them, not a sample: completed with zeros past their end, some of
  // KLEE's partial paths reach the error when the fuzzer runs them -- the
  // dry run records them as crashes (sig 06), and that is how the fixed
  // hybrid covered eca-* tasks KLEE itself left UNKNOWN. A cap of the 64
  // latest dropped exactly those (R19: 5 eca-* losses of --alternate-engines).
  // Calibrating thousands is cheap now that a nondet loop ends on zeros.
  std::sort(tests.begin(), tests.end());
  unsigned written = 0;
  unsigned index = 0;
  for (const std::string &test : tests) {
    std::vector<Map2Check::KtestObject> objects =
        Map2Check::readKtestFile(test);
    if (objects.empty()) continue;

    std::vector<uint8_t> bytes = Map2Check::ktestToFuzzerBytes(objects);
    if (bytes.empty()) continue;

    // Named by index rather than by content hash: AFL++ renames what it
    // keeps to its own hash anyway, so a second one here buys nothing.
    std::ostringstream name;
    name << aflSeeds << "/klee-" << index++;
    std::ofstream out(name.str(), std::ios::binary);
    if (!out.is_open()) continue;
    out.write(reinterpret_cast<const char *>(bytes.data()),
              static_cast<std::streamsize>(bytes.size()));
    if (out.good()) ++written;
  }
  if (written > 0) {
    Map2Check::Log::Info("Seeded the fuzzer corpus with " +
                         std::to_string(written) + " vectors from KLEE");
  }
  return written;
}

unsigned Caller::exportFuzzerCorpusAsKtests() {
  // Each queue entry is replayed through the witness binary, whose runtime
  // logs every nondet read with its type (klee_log.csv) -- which is what a
  // .ktest needs and what the fuzzer's raw bytes lack. The replay runs in the
  // store's replay/, not here: the witness writes map2check_property and
  // friends into its working directory, and a replay must never overwrite a
  // violation this phase already recorded.
  std::error_code error;
  const std::string queue = "afl-out/default/queue";
  const std::string replay = seedStore + "/replay";
  const std::string ktests = seedStore + "/ktest";
  const std::string witness =
      std::filesystem::absolute(programHash + "-witness-fuzzed.out", error)
          .string();
  if (!std::filesystem::exists(witness, error)) return 0;

  std::vector<std::string> names;
  for (const auto &entry :
       std::filesystem::directory_iterator(queue, error)) {
    if (entry.is_regular_file(error)) {
      names.push_back(entry.path().filename().string());
    }
  }
  // This round's discoveries replace the last round's: KLEE already ran on
  // those, and its own tests from that turn (kleeprev/) carry them forward.
  std::filesystem::remove_all(ktests, error);
  std::filesystem::create_directories(ktests, error);

  // Bounded as a whole, not only per entry: the replays come out of the KLEE
  // phase's budget, so they stop at 5% of the run's budget (at least 2 s).
  const auto started = std::chrono::steady_clock::now();
  const auto allowed = std::chrono::duration<double>(
      std::max(2.0, 0.05 * static_cast<double>(this->timeout)));
  std::set<std::vector<uint8_t>> seen;
  unsigned written = 0;
  for (const std::string &name :
       Map2Check::selectQueueEntries(names, kMaxSeedsFromFuzzer)) {
    if (std::chrono::steady_clock::now() - started >= allowed) break;
    std::filesystem::remove_all(replay, error);
    std::filesystem::create_directories(replay, error);
    const std::string input =
        std::filesystem::absolute(queue + "/" + name, error).string();
    std::ostringstream command;
    command << "cd '" << replay << "' && MAP2CHECK_SEED_REPLAY=1 timeout -k 1 2 '" << witness
            << "' < '" << input << "' > /dev/null 2>&1";
    system(command.str().c_str());

    const std::vector<Map2Check::KtestObject> objects =
        Map2Check::readNonDetLogAsObjects(replay + "/" +
                                          Map2Check::kleeLogCSV);
    if (objects.empty()) continue;
    if (!Map2Check::isNewVector(Map2Check::ktestToFuzzerBytes(objects),
                                &seen)) {
      continue;
    }
    if (Map2Check::writeKtestFile(
            ktests + "/afl-" + std::to_string(written) + ".ktest", objects)) {
      ++written;
    }
  }
  std::filesystem::remove_all(replay, error);
  if (written > 0) {
    Map2Check::Log::Info("Seeded KLEE with " + std::to_string(written) +
                         " vectors from AFL++");
  }
  return written;
}

namespace {
/** MAP2CHECK_SLICER_FLAGS / MAP2CHECK_SLICE_CLEANUP: experiment knobs (tacas
 * 2d spec), read from the environment so a campaign arm can set them without
 * a CLI option nobody else should use. */
std::string environmentKnob(const char *name) {
  const char *value = std::getenv(name);
  return value == nullptr ? std::string() : std::string(value);
}
}  // namespace

std::vector<std::vector<std::string>> Caller::fuzzerCorpusVectors(
    size_t cap) {
  std::vector<std::vector<std::string>> vectors;
  std::error_code error;
  const std::string queue = "afl-out/default/queue";
  const std::string replay =
      std::filesystem::absolute("suite-replay", error).string();
  const std::string witness =
      std::filesystem::absolute(programHash + "-witness-fuzzed.out", error)
          .string();
  if (!std::filesystem::exists(witness, error)) return vectors;

  std::vector<std::string> names;
  for (const auto &entry : std::filesystem::directory_iterator(queue, error)) {
    if (entry.is_regular_file(error)) {
      names.push_back(entry.path().filename().string());
    }
  }
  const auto started = std::chrono::steady_clock::now();
  const auto allowed = std::chrono::duration<double>(
      std::max(2.0, 0.05 * static_cast<double>(this->timeout)));
  std::set<std::vector<std::string>> seen;
  for (const std::string &name : Map2Check::selectQueueEntries(names, cap)) {
    if (std::chrono::steady_clock::now() - started >= allowed) break;
    std::filesystem::remove_all(replay, error);
    std::filesystem::create_directories(replay, error);
    const std::string input =
        std::filesystem::absolute(queue + "/" + name, error).string();
    std::ostringstream command;
    command << "cd '" << replay
            << "' && MAP2CHECK_SEED_REPLAY=1 timeout -k 1 2 '" << witness
            << "' < '" << input << "' > /dev/null 2>&1";
    system(command.str().c_str());
    std::vector<std::string> values =
        Map2Check::readNonDetLog(replay + "/" + Map2Check::kleeLogCSV);
    if (values.empty() || !seen.insert(values).second) continue;
    vectors.push_back(values);
  }
  std::filesystem::remove_all(replay, error);
  return vectors;
}

bool Caller::runSlicer(const std::string &input, const std::string &output,
                       std::vector<std::string> primary, bool addRuntimeNames,
                       const std::string &entry, const std::string &label) {
  // Sliced once per run, not once per phase: see Map2Check::sliceCachePath.
  std::error_code error;
  std::string key;
  {
    std::ifstream in(input, std::ios::binary);
    if (in.is_open()) {
      std::stringstream content;
      content << in.rdbuf();
      key = Map2Check::sliceCacheKey(
          content.str(), label, entry,
          environmentKnob("MAP2CHECK_SLICER_FLAGS") + "|" +
              environmentKnob("MAP2CHECK_SLICE_CLEANUP"));
    }
  }
  const std::string cached = sliceCache + "/" + key + ".bc";
  const std::string failed = sliceCache + "/" + key + ".failed";
  if (!key.empty() && std::filesystem::exists(failed, error)) {
    Map2Check::Log::Warning(
        "sbt-slicer produced no usable output (cached from an earlier phase) "
        "-- analysing the unsliced program");
    return false;
  }
  if (!key.empty() && std::filesystem::exists(cached, error) &&
      std::filesystem::copy_file(
          cached, output, std::filesystem::copy_options::overwrite_existing,
          error)) {
    Map2Check::Log::Info("Slice of " + label +
                         ": reusing the slice from an earlier phase");
    return true;
  }

  const bool sliced =
      runSlicerUncached(input, output, primary, addRuntimeNames, entry, label);
  if (sliced) cleanUpSlice(output);
  if (!key.empty() && std::filesystem::exists(Map2Check::slicerBinary())) {
    std::filesystem::create_directories(sliceCache, error);
    if (sliced) {
      std::filesystem::copy_file(
          output, cached, std::filesystem::copy_options::overwrite_existing,
          error);
    } else {
      std::ofstream(failed) << label << "\n";
    }
  }
  return sliced;
}

void Caller::cleanUpSlice(const std::string &slice) {
  const std::string passes = Map2Check::sliceCleanupPasses(
      environmentKnob("MAP2CHECK_SLICE_CLEANUP"));
  if (passes.empty()) return;
  const std::string cleaned = slice + ".clean.bc";
  std::ostringstream command;
  // Bounded like the slicer: a pass pipeline over a large module is not
  // free, and it runs outside any engine's window.
  const unsigned cleanupBudget = static_cast<unsigned>(std::max(
      2.0, std::min(0.05 * this->timeout,
                    static_cast<double>(remainingSeconds()) - 5.0)));
  command << "timeout -k " << Map2Check::killGracePeriod << " "
          << cleanupBudget << " " << Map2Check::optBinary << " " << passes
          << " " << slice << " -o " << cleaned << " >> slicer.output 2>&1";
  Map2Check::Log::Debug(command.str());
  std::error_code error;
  if (system(command.str().c_str()) == 0 &&
      std::filesystem::exists(cleaned, error) &&
      std::filesystem::file_size(cleaned, error) > 0) {
    std::filesystem::rename(cleaned, slice, error);
    if (!error) return;
  }
  Map2Check::Log::Warning("could not clean up the slice (" + passes +
                          ") -- keeping it as sliced");
}

bool Caller::runSlicerUncached(const std::string &input,
                               const std::string &output,
                               std::vector<std::string> primary,
                               bool addRuntimeNames, const std::string &entry,
                               const std::string &label) {
  const std::string slicer = Map2Check::slicerBinary();
  if (!std::filesystem::exists(slicer)) {
    // Announced, not silently skipped. A slicer that is asked for and absent
    // must not leave the run quietly analysing the whole program: that is how
    // --add-invariants stayed dead through a full baseline (issue #54).
    Map2Check::Log::Warning(
        "slicing was requested but sbt-slicer is not installed at " + slicer +
        " -- analysing the unsliced program");
    return false;
  }
  if (!std::filesystem::exists(input)) return false;

  // Bounded, for the reason every other external step here is bounded: the
  // slicer builds a system dependence graph over the whole module, and on the
  // large programs that is not fast. A slice that does not finish is not a
  // loss -- the caller falls back to the whole program; overrunning the budget
  // loses the verdict instead.
  const double sliceBudget = std::max(
      1.0,
      std::min(0.2 * this->timeout,
               std::max(1.0, static_cast<double>(remainingSeconds()) - 5.0)));

  // The program's own names join the criteria: the nondet functions (no fixed
  // list knows every name a benchmark declares, and a missing one silently
  // shifts the suite), and for the memory properties every map2check_* call
  // the instrumentation inserted. Read from the textual IR -- the bitcode
  // string table packs names with no separator. If the disassembly fails, the
  // fixed nondet list stands.
  const std::string inputIR = input + ".ll";
  std::ostringstream disassemble;
  disassemble << Map2Check::optBinary << " -S " << input << " -o " << inputIR
              << " > /dev/null 2>&1";
  std::vector<std::string> programNondets;
  std::string irText;
  if (system(disassemble.str().c_str()) == 0) {
    std::ifstream irFile(inputIR);
    std::stringstream irStream;
    irStream << irFile.rdbuf();
    irText = irStream.str();
    programNondets = Map2Check::nondetNamesInIR(irText);
  }
  if (addRuntimeNames) {
    // The runtime checks decide the property and external calls can commit
    // the error themselves: both are criteria. Without the IR there are no
    // checks to keep, and a slice would remove them all -- refuse instead of
    // falling back to the nondet list, which is only sound for reach/assert.
    std::vector<std::string> runtimeCriteria;
    if (!Map2Check::instrumentedSliceCriteria(irText, &runtimeCriteria)) {
      Map2Check::Log::Warning(
          "could not read the instrumented module's runtime calls -- "
          "analysing the unsliced program");
      return false;
    }
    for (const std::string &name : runtimeCriteria) primary.push_back(name);
  }

  // -cutoff-diverging=false: the cutoff rewrites every path that cannot reach
  // the criterion into exit(0) with no debug location, and once KLEE links
  // uClibc the verifier rejects the module ("Broken module found") -- KLEE
  // never ran on a sliced task with a cut path (tacasv2a spec, defect 1).
  // --statistics: counts before and after, logged below.
  std::ostringstream command;
  command << "timeout -k " << Map2Check::killGracePeriod << " "
          << static_cast<unsigned>(sliceBudget) << " " << slicer << " -c "
          << Map2Check::slicingCriteria(primary, programNondets)
          << " --entry=" << entry
          << " -cutoff-diverging=false --statistics "
          << environmentKnob("MAP2CHECK_SLICER_FLAGS") << " -o " << output << " "
          << input << " > slicer.output 2>&1";
  Map2Check::Log::Debug(command.str());
  const int result = system(command.str().c_str());

  std::error_code error;
  const bool produced = std::filesystem::exists(output, error) &&
                        std::filesystem::file_size(output, error) > 0;
  if (result != 0 || !produced) {
    Map2Check::Log::Warning(
        "sbt-slicer produced no usable output -- analysing the unsliced "
        "program");
    return false;
  }

  // Reported, because a slice is not a neutral speed-up: it narrows the
  // question being answered, and this line is the only visible sign of how
  // much was dropped.
  const auto before = std::filesystem::file_size(input, error);
  const auto after = std::filesystem::file_size(output, error);
  std::ifstream slicerLog("slicer.output");
  std::stringstream slicerText;
  slicerText << slicerLog.rdbuf();
  Map2Check::Log::Info(Map2Check::describeSlice(
      label, Map2Check::parseSlicerStatistics(slicerText.str()), before,
      after));
  return true;
}

bool Caller::sliceWithRespectToTarget(const std::string &targetFunction,
                                      const std::vector<std::string> &criteria) {
  // The COMPILED bitcode, not the instrumented one: this runs before callPass
  // so that the instrumentation is applied to the slice rather than removed by
  // it. Entry is still plain main at this point, for the same reason.
  const std::string input = programHash + "-compiled.bc";
  const std::string output = programHash + "-sliced.bc";
  std::string label;
  for (const std::string &name : criteria) {
    label += (label.empty() ? "" : ",") + name;
  }
  if (!runSlicer(input, output, criteria, false, "main", label)) return false;
  std::error_code error;

  // sbt-slicer removes the body of the criterion function itself. reach_error
  // is where the slice ENDS -- nothing it does can influence whether it is
  // reached -- so the slicer keeps the call site and drops the definition.
  //
  // KLEE tolerates the resulting declaration. The native AFL++ link does
  // not: it fails with "undefined reference to reach_error", no *-fuzzed.out
  // is produced, and the fuzzer stage then does nothing at all.
  //
  // A WEAK definition restores the link without displacing a real one: where
  // the slice did keep the body, the strong definition still wins.
  const std::string stubSource = programHash + "-target-stub.c";
  const std::string stubBitcode = programHash + "-target-stub.bc";
  const std::string linked = programHash + "-sliced-linked.bc";
  {
    std::ofstream stub(stubSource);
    if (stub.is_open()) {
      stub << Map2Check::targetStubSource(targetFunction);
    }
  }
  std::ostringstream compileStub;
  compileStub << Map2Check::clangBinary << " -Wno-everything -c -emit-llvm -g"
              << " " << Caller::preOptimizationFlags() << " -o " << stubBitcode
              << " " << stubSource << " >> slicer.output 2>&1";
  std::ostringstream linkStub;
  linkStub << Map2Check::llvmLinkBinary << " " << output << " " << stubBitcode
           << " -o " << linked << " >> slicer.output 2>&1";
  if (system(compileStub.str().c_str()) == 0 &&
      system(linkStub.str().c_str()) == 0 &&
      std::filesystem::exists(linked, error) &&
      std::filesystem::file_size(linked, error) > 0) {
    std::filesystem::rename(linked, output, error);
  } else {
    // Not fatal: without the stub the fuzzer stage is lost, but KLEE still
    // runs on the slice. Say so rather than returning a half-configured run.
    Map2Check::Log::Warning(
        "could not restore a definition of " + targetFunction +
        " after slicing -- the AFL++ stage will not link");
  }

  std::filesystem::rename(output, input, error);
  return !error;
}

bool Caller::sliceInstrumented() {
  // Memory properties have no criterion in the user's program: the property
  // is decided by the runtime calls MemoryTrackPass inserted, so the slice is
  // taken after instrumentation with every map2check_* call as a criterion
  // (none can be dropped), and __map2check_main__ -- the renamed user main --
  // as the entry. tacasv2b spec, section 2.
  const std::string input = programHash + "-output.bc";
  const std::string output = programHash + "-sliced-instrumented.bc";
  if (!std::filesystem::exists(input)) return false;
  if (!runSlicer(input, output, {}, true, "__map2check_main__",
                 "map2check runtime")) {
    return false;
  }
  std::error_code error;
  std::filesystem::rename(output, input, error);
  return !error;
}

void Caller::restoreWorkingDirectory() {
  std::error_code error;
  std::filesystem::current_path(currentPath, error);
}

void Caller::cleanGarbage() {
  std::filesystem::current_path(currentPath);
  std::ostringstream removeCommand;
  removeCommand.str("");
  removeCommand << "rm -rf " << programHash;
  Map2Check::Log::Debug("Remove " + removeCommand.str());
  system(removeCommand.str().c_str());
}

void Caller::applyNonDetGenerator() {
  switch (nonDetGenerator) {
    case (NonDetGenerator::None): {  // TODO(hbgit): Should generate binary
      Map2Check::Log::Info(
          "Map2Check will not generate non deterministic numbers");
      break;
    }
    case (NonDetGenerator::Klee): {
      Map2Check::Log::Info("Applying optimizations for klee");
      break;
    }
    case (NonDetGenerator::AFLPlusPlus): {
      Map2Check::Log::Info("Instrumenting with AFL++");
      // Built once per run: every fuzzer phase recompiles the same modules,
      // and on the eca-* programs the three builds take ~24 s -- per phase,
      // which under --alternate-engines was most of each fuzzer turn. Keyed by
      // the two modules' content, like the slice cache.
      const std::vector<std::string> aflBinaries = {
          programHash + "-fuzzed.out", programHash + "-witness-fuzzed.out",
          programHash + "-cmplog.out"};
      std::string buildKey;
      {
        std::ifstream result(programHash + "-result.bc", std::ios::binary);
        std::ifstream witness(programHash + "-witness-result.bc",
                              std::ios::binary);
        std::stringstream content;
        content << result.rdbuf() << "|" << witness.rdbuf();
        buildKey = Map2Check::sliceCacheKey(content.str(), "afl++", "", "");
      }
      const std::string builtDir = buildCache + "/" + buildKey;
      {
        std::error_code cacheErr;
        if (std::filesystem::exists(builtDir + "/" + aflBinaries[0],
                                    cacheErr)) {
          for (const std::string &binary : aflBinaries) {
            if (std::filesystem::exists(builtDir + "/" + binary, cacheErr)) {
              std::filesystem::copy_file(
                  builtDir + "/" + binary, binary,
                  std::filesystem::copy_options::overwrite_existing, cacheErr);
            }
          }
          Map2Check::Log::Info(
              "AFL++ binaries: reusing the build of an earlier phase");
          break;
        }
      }
      std::ostringstream command;
      command.str("");

      // Bounded, because it was not, and that is where the budget went.
      //
      // Both invocations run clang at -O2 over the whole instrumented module.
      // On the large ECA and Recursive programs that takes longer than the
      // entire budget, and nothing was stopping it: the run was still linking
      // its fuzzer binary when the harness's outer timeout killed it, so it
      // produced no verdict and scored ERROR. Measured on the v12 corpus:
      // 20 such tasks, every one at 87 to 89 seconds against a 60 second
      // budget, all of them dying at this exact step.
      //
      // Losing the fuzzer binary is a real cost, but a bounded one: KLEE still
      // gets its phase and the run still reaches a verdict. Spending the whole
      // budget here costs the verdict itself.
      const double compileBudget = std::max(
          1.0, std::min(0.25 * this->timeout,
                        std::max(1.0, static_cast<double>(remainingSeconds()) -
                                          5.0)));
      // One budget for the three builds, not one each: sequential, each
      // bounded by 0.25T, they could take 0.75T -- on eca-* that ran the
      // process past its deadline with no verdict (R19). Each build gets what
      // is left of the shared deadline.
      const auto buildDeadline =
          std::chrono::steady_clock::now() +
          std::chrono::duration<double>(compileBudget);
      auto bound = [&buildDeadline]() {
        const double left = std::chrono::duration<double>(
                                buildDeadline - std::chrono::steady_clock::now())
                                .count();
        return "timeout -k " + std::to_string(Map2Check::killGracePeriod) +
               " " + std::to_string(std::max(1, static_cast<int>(left))) + " ";
      };

      command
          << bound() << Map2Check::aflClangFastBinary()
          << "  -g " << Caller::postOptimizationFlags()
          << " -o " + programHash + "-fuzzed.out"
          << " " + programHash + "-result.bc" << " > afl-build.log 2>&1";

      const int built = system(command.str().c_str());

      std::ostringstream commandWitness;
      commandWitness.str("");
      commandWitness << bound() << Map2Check::aflClangFastBinary()
                     << "  -g "
                     << " -o " + programHash + "-witness-fuzzed.out"
                     << " " + programHash + "-witness-result.bc";

      system(commandWitness.str().c_str());

      // The CmpLog companion binary: the same program instrumented to log
      // the operands of comparisons, which afl-fuzz (-c) uses to solve
      // magic-value guards such as `x == 123456` by input-to-state
      // substitution. It is AFL++'s counterpart of the value profile the
      // previous fuzzer ran with (-use_value_profile=1); without it the
      // tacasv1 comparison would pit an unarmed AFL++ against an armed
      // LibFuzzer. Optional: if it does not build, the fuzzer runs without.
      std::ostringstream commandCmplog;
      commandCmplog << "AFL_LLVM_CMPLOG=1 " << bound()
                    << Map2Check::aflClangFastBinary() << "  -g "
                    << Caller::postOptimizationFlags()
                    << " -o " + programHash + "-cmplog.out"
                    << " " + programHash + "-result.bc";
      system(commandCmplog.str().c_str());

      {
        std::error_code cacheErr;
        if (std::filesystem::exists(aflBinaries[0], cacheErr)) {
          std::filesystem::create_directories(builtDir, cacheErr);
          for (const std::string &binary : aflBinaries) {
            if (std::filesystem::exists(binary, cacheErr)) {
              std::filesystem::copy_file(
                  binary, builtDir + "/" + binary,
                  std::filesystem::copy_options::overwrite_existing, cacheErr);
            }
          }
        }
      }

      // Announced rather than discovered later as a silent no-op -- the same
      // failure mode the sliced arm spent a whole campaign in. And for what it
      // is: a link error (an undefined reach_error) used to be reported as a
      // build that ran out of time.
      std::error_code fuzzErr;
      if (!std::filesystem::exists(programHash + "-fuzzed.out", fuzzErr)) {
        std::string why;
        if (built == 31744) {  // timeout's 124
          why = "did not build within " +
                std::to_string(static_cast<int>(compileBudget)) + "s";
        } else {
          std::ifstream buildLog("afl-build.log");
          // The first complaint names the cause (an undefined reference);
          // the last is only clang's "linker command failed".
          std::string line, firstError;
          while (firstError.empty() && std::getline(buildLog, line)) {
            if (line.find("error") != std::string::npos ||
                line.find("undefined reference") != std::string::npos) {
              firstError = line;
            }
          }
          why = "failed to build (status " + std::to_string(built) + ")" +
                (firstError.empty() ? "" : ": " + firstError);
        }
        Map2Check::Log::Warning(
            "the AFL++ binary " + why +
            " -- skipping the fuzzer phase and leaving the budget to KLEE");
      } else if (!std::filesystem::exists(programHash + "-cmplog.out",
                                          fuzzErr)) {
        Map2Check::Log::Warning(
            "the AFL++ CmpLog binary did not build -- fuzzing without "
            "comparison solving");
      }
      break;
    }
  }
}

int Caller::callPass(std::string target_function, bool sv_comp) {
  std::ostringstream transformCommand;
  transformCommand.str("");
  transformCommand << Map2Check::optBinary;

  // --- New Pass Manager: use -load-pass-plugin + -passes= ---
  std::string nonDetPlugin = "${MAP2CHECK_PATH}/lib/libNonDetPass";

  Map2Check::Log::Info("Adding nondet pass");
  transformCommand << " -tailcallopt";
  transformCommand << " -load-pass-plugin=" << nonDetPlugin << getLibSuffix();

  // Build the passes pipeline
  std::ostringstream passesArg;
  passesArg << "nondet-pass";

  bool loadsMemoryTrackPass = false;
  switch (map2checkMode) {
    case Map2CheckMode::MEMTRACK_MODE: {
      Map2Check::Log::Info("Adding memtrack pass");
      std::string memPlugin = "${MAP2CHECK_PATH}/lib/libMemoryTrackPass";
      transformCommand << " -load-pass-plugin=" << memPlugin << getLibSuffix();
      passesArg << ",memory-track";
      loadsMemoryTrackPass = true;
      break;
    }
    case Map2CheckMode::MEMCLEANUP_MODE: {
      Map2Check::Log::Info("Adding memcleanup pass");
      std::string memPlugin = "${MAP2CHECK_PATH}/lib/libMemoryTrackPass";
      transformCommand << " -load-pass-plugin=" << memPlugin << getLibSuffix();
      passesArg << ",memory-track";
      loadsMemoryTrackPass = true;
      break;
    }
    case Map2CheckMode::OVERFLOW_MODE: {
      std::string overflowPlugin = "${MAP2CHECK_PATH}/lib/libOverflowPass";
      transformCommand << " -load-pass-plugin=" << overflowPlugin
                       << getLibSuffix();
      passesArg << ",overflow-pass";
      break;
    }
    case Map2CheckMode::COVER_BRANCHES_MODE: {
      // Nothing beyond nondet-pass. The inputs have to be recorded, so that
      // pass stays; there is no property to instrument for, so nothing else
      // does. This is also the leanest pipeline the tool has, which is the
      // point: fewer instrumented calls means KLEE explores more paths in the
      // same budget, and paths are what a branch suite is made of.
      Map2Check::Log::Info("Running cover-branches mode (no property check)");
      break;
    }
    case Map2CheckMode::REACHABILITY_MODE: {
      Map2Check::Log::Info("Running reachability mode");
      Map2Check::Log::Debug("Target function: " + target_function);
      std::string targetPlugin = "${MAP2CHECK_PATH}/lib/libTargetPass";
      transformCommand << " -load-pass-plugin=" << targetPlugin
                       << getLibSuffix();
      // Pass target function name via cl::opt flag to opt
      transformCommand << " -function-name=" << target_function;
      passesArg << ",target-pass";
      break;
    }
    case Map2CheckMode::ASSERT_MODE: {
      Map2Check::Log::Info("Running assert mode");
      std::string assertPlugin = "${MAP2CHECK_PATH}/lib/libAssertPass";
      transformCommand << " -load-pass-plugin=" << assertPlugin
                       << getLibSuffix();
      passesArg << ",assert-pass";
      break;
    }
    default: { break; }
  }

  Map2Check::Log::Info("Adding map2check pass");
  std::string map2checkPlugin = "${MAP2CHECK_PATH}/lib/libMap2CheckLibrary";
  transformCommand << " -load-pass-plugin=" << map2checkPlugin
                   << getLibSuffix();
  passesArg << ",map2check-library";

  transformCommand << " -entry-function=" << this->entryFunction;
  if (loadsMemoryTrackPass) {
    transformCommand << " -m2c-entry-function=" << this->entryFunction;
  }
  if (this->wasmMode) {
    transformCommand << " -wasm-mode";
  }

  transformCommand << " -passes='" << passesArg.str() << "'";

  std::string input_file = "< " + this->pathprogram;
  std::string output_file = "> " + programHash + "-output.bc";

  transformCommand << input_file << output_file;
  Map2Check::Log::Debug(transformCommand.str());

  system(transformCommand.str().c_str());

  return 1;
}

void Caller::linkLLVM() {
  /* Link functions called after executing the passes */
  // TODO(rafa.sa.xp@gmail.com) Only link against used libraries

  Map2Check::Log::Info("Linking with map2check library");

  std::ostringstream witnessCommand;
  std::ostringstream linkCommand;
  linkCommand.str("");
  linkCommand << Map2Check::llvmLinkBinary;
  linkCommand << " " + programHash + "-output.bc"
              << " ${MAP2CHECK_PATH}/lib/Map2CheckFunctions.bc"
              << " ${MAP2CHECK_PATH}/lib/TrackBBLog.bc"
              << " ${MAP2CHECK_PATH}/lib/NonDetLog.bc"
              << " ${MAP2CHECK_PATH}/lib/PropertyGenerator.bc";

  switch (dataStructure) {
    case DataStructure::Array: {
      linkCommand << " ${MAP2CHECK_PATH}/lib/ContainerRealloc.bc";
      break;
    }
    case DataStructure::BTree: {
      linkCommand << " ${MAP2CHECK_PATH}/lib/ContainerBTree.bc"
                  << " ${MAP2CHECK_PATH}/lib/BTree.bc";
      break;
    }
  }

  switch (map2checkMode) {
    case Map2CheckMode::MEMTRACK_MODE: {
      linkCommand << " ${MAP2CHECK_PATH}/lib/AnalysisModeMemtrack.bc"
                  << " ${MAP2CHECK_PATH}/lib/AllocationLog.bc"
                  << " ${MAP2CHECK_PATH}/lib/ListLog.bc"
                  << " ${MAP2CHECK_PATH}/lib/HeapLog.bc";
      break;
    }
    case Map2CheckMode::MEMCLEANUP_MODE: {
      linkCommand << " ${MAP2CHECK_PATH}/lib/AnalysisModeMemcleanup.bc"
                  << " ${MAP2CHECK_PATH}/lib/AllocationLog.bc"
                  << " ${MAP2CHECK_PATH}/lib/ListLog.bc"
                  << " ${MAP2CHECK_PATH}/lib/HeapLog.bc";
      break;
    }
    case Map2CheckMode::OVERFLOW_MODE: {
      linkCommand << " ${MAP2CHECK_PATH}/lib/AnalysisModeOverflow.bc";
      break;
    }
    case Map2CheckMode::ASSERT_MODE: {
      linkCommand << " ${MAP2CHECK_PATH}/lib/AnalysisModeAssert.bc";
      break;
    }
    case Map2CheckMode::REACHABILITY_MODE: {
      // Since the map2check api provides the function, we do not need to do any
      // analysis
      linkCommand << " ${MAP2CHECK_PATH}/lib/AnalysisModeNone.bc";
      break;
    }
    case Map2CheckMode::COVER_BRANCHES_MODE: {
      // No property, so no analysis -- the run exists to explore and record.
      linkCommand << " ${MAP2CHECK_PATH}/lib/AnalysisModeNone.bc";
      break;
    }
  }

  switch (nonDetGenerator) {
    case (NonDetGenerator::None): {
      linkCommand << " ${MAP2CHECK_PATH}/lib/NonDetGeneratorNone.bc";
      break;
    }
    case (NonDetGenerator::Klee): {  // TODO(hbgit): Add klee non det generator
      linkCommand << " ${MAP2CHECK_PATH}/lib/NonDetGeneratorKlee.bc";
      break;
    }
    case (NonDetGenerator::AFLPlusPlus): {
      linkCommand << " ${MAP2CHECK_PATH}/lib/NonDetGeneratorAFL.bc";
      break;
    }
  }

  if (this->wasmMode) {
    linkCommand << " ${MAP2CHECK_PATH}/lib/WasmRuntimeStubs.bc";
  }

  witnessCommand.str("");
  witnessCommand << linkCommand.str();
  witnessCommand << " ${MAP2CHECK_PATH}/lib/WitnessGeneration.bc";
  witnessCommand << "  > " + programHash + "-witness-result.bc";
  Map2Check::Log::Debug(witnessCommand.str());
  system(witnessCommand.str().c_str());

  linkCommand << " ${MAP2CHECK_PATH}/lib/WitnessGenerationNone.bc";
  linkCommand << "  > " + programHash + "-result.bc";
  Map2Check::Log::Debug(linkCommand.str());
  system(linkCommand.str().c_str());
}

std::string Caller::generateWasmWrapperStatic(const std::string& wasmOutHeaderPath,
                                               const std::string& entryPointName) {
  // Parse entry point name to extract module prefix
  // e.g., w2c_0x24test__array0x2Ewasm_0x5Fstart → module is 0x24test__array0x2Ewasm
  std::string moduleName;
  std::string typeName = entryPointName;
  if (entryPointName.size() > 4 && entryPointName.substr(0, 4) == "w2c_") {
    size_t pos = entryPointName.rfind("_0x5Fstart");
    if (pos != std::string::npos) {
      moduleName = entryPointName.substr(4, pos - 4);
      typeName = "w2c_" + moduleName;
    }
  }

  // Use a temp file for the wrapper C source
  char tmpPath[] = "/tmp/m2c_wasm_wrapper_XXXXXX";
  int fd = mkstemp(tmpPath);
  if (fd < 0) return "";
  close(fd);
  std::string wrapperPath = std::string(tmpPath) + ".c";
  std::string wrapperBcPath = std::string(tmpPath) + ".bc";

  std::ostringstream wrapper;
  wrapper << "#include \"" << wasmOutHeaderPath << "\"\n"
          << "#include <stdlib.h>\n"
          << "int main() {\n"
          << "    " << typeName << " instance;\n"
          << "    wasm2c_" << moduleName << "_instantiate(&instance, NULL);\n"
          << "    " << entryPointName << "(&instance);\n"
          << "    wasm2c_" << moduleName << "_free(&instance);\n"
          << "    return 0;\n"
          << "}\n";

  std::ofstream outFile(wrapperPath);
  outFile << wrapper.str();
  outFile.close();

  std::string headerDir = wasmOutHeaderPath.substr(0, wasmOutHeaderPath.find_last_of("/"));
  std::string wasmIncludePath;
  struct stat st;
  if (stat("/opt/wabt-1.0.41/include", &st) == 0) {
    wasmIncludePath = "/opt/wabt-1.0.41/include";
  } else if (stat("/opt/wabt/include", &st) == 0) {
    wasmIncludePath = "/opt/wabt/include";
  } else {
    wasmIncludePath = "/usr/include";
  }
  std::ostringstream compileCmd;
  compileCmd << Map2Check::clangBinary << " -c -emit-llvm"
             << " -I" << wasmIncludePath
             << " -I" << headerDir
             << " " << wrapperPath << " -o " << wrapperBcPath;
  system(compileCmd.str().c_str());

  return wrapperBcPath;
}

void Caller::executeAnalysis(std::string solvername) {
  switch (nonDetGenerator) {
    // TODO(hbgit): implement this method
    case (NonDetGenerator::None): {  // TODO(hbgit): Activate mode
      Map2Check::Log::Info("This mode is not supported");
      break;
    }
    case (NonDetGenerator::Klee): {
      Map2Check::Log::Info("Executing Klee with map2check");
      std::ostringstream kleeCommand;
      kleeCommand.str("");
      // -k: KLEE installs a SIGTERM handler that tries to shut down gracefully,
      // but that handler never runs while the solver is wedged. Plain `timeout`
      // then waits forever for a child that will not die, and map2check hangs
      // past its own budget. The grace period escalates to SIGKILL so the
      // budget is actually enforced.
      // Reserve a few seconds for what happens AFTER the engine: reading the
      // property file, recovering the input vector, writing the suite. KLEE
      // holding the budget to its last second is what turned a decided run
      // into an ERROR.
      constexpr double kPostEngineReserve = 5.0;
      // With the exchange on, the fuzzer gets a real third phase: 0.2 / 0.6 /
      // 0.2. Without it the hybrid keeps the 0.2 / 0.8 it was measured with.
      // Alternating (--alternate-engines), main hands each phase its window.
      const double kleeShare = this->seedExchange ? 0.6 : 0.8;
      const double kleeCap =
          this->engineWindow > 0 ? this->engineWindow : kleeShare * this->timeout;
      const double kleeBudget = std::max(
          1.0, std::min(kleeCap, this->remainingSeconds() - kPostEngineReserve));
      kleeCommand << "timeout -k " << Map2Check::killGracePeriod << " "
                  << static_cast<unsigned>(kleeBudget) << " ";
      kleeCommand << Map2Check::kleeBinary;

      // KLEE's own deadline, set BELOW the external one so it is KLEE that
      // stops, not the kill.
      //
      // Without it every path explored up to the budget is thrown away.
      // Measured: a program with twelve nondeterministic reads explored 3982
      // paths and produced ZERO .ktest files, because `timeout` killed KLEE
      // before it wrote any -- and those files are where a Cover-Branches
      // suite comes from, and where a Cover-Error suite now recovers its
      // input vector. Reaching the budget is the NORMAL case in a competition
      // run, so this was not an edge: it was the common path discarding all
      // of its work.
      //
      // The external timeout above stays as the backstop for the case its
      // comment describes, a solver wedged so deep that KLEE's own deadline
      // never gets a turn.
      // INTEGER seconds. KLEE parses --max-time with a duration parser that
      // rejects a fractional value outright:
      //
      //     KLEE: ERROR: Illegal number format: 36.75s
      //
      // and exits 256 without running a single instruction. The old expression
      // 0.7*timeout happened to be whole for every budget in use, so this
      // never showed; 0.875*kleeBudget is whole only when kleeBudget is a
      // multiple of 8, and kleeBudget now derives from the time REMAINING,
      // which is whatever the clock says.
      //
      // The cost of getting this wrong is total and silent: no KLEE phase at
      // all, so nothing can be proved safe. It collapsed 464 Juliet TN
      // verdicts to 1.
      kleeCommand << " --max-time="
                  << std::max(1u, static_cast<unsigned>(0.875 * kleeBudget))
                  << "s";


      // Halting on the first error is right when there is a property to
      // decide -- the answer is known, and more exploration is waste. It is
      // wrong for Cover-Branches, where there is no property and the paths ARE
      // the product: stopping at the first error throws away every path not
      // yet explored, and with it the test cases they would have produced.
      //
      // Measured before this: the same twelve-branch program produced 1020
      // .ktest files on one run and zero on the next, the difference being
      // whether an error happened to be hit early. A suite that depends on
      // that is not a suite.
      // A starting point from whatever ran before, when there is one. KLEE
      // replays the seed and then explores around it, instead of rediscovering
      // from nothing a path the fuzzer already walked -- which under a fixed
      // budget is not merely faster, it is depth the run would not otherwise
      // have reached.
      // KLEE starts from the fuzzer's corpus when there is one, replaying
      // each seed and exploring around it instead of rediscovering from
      // nothing the paths the fuzzer already walked. --seed-time caps the
      // replay at a quarter of the phase, so seeding cannot eat the search.
      std::string seedFlag;
      if (this->seedExchange) {
        std::error_code seedError;
        bool haveSeeds = false;
        for (const auto &entry : std::filesystem::directory_iterator(
                 seedStore + "/ktest", seedError)) {
          if (entry.is_regular_file(seedError)) {
            haveSeeds = true;
            break;
          }
        }
        // KLEE's own tests from its previous turn, when the engines alternate:
        // replaying them rebuilds the frontier it had reached instead of
        // rediscovering it.
        bool haveOwnSeeds = false;
        for (const auto &entry : std::filesystem::directory_iterator(
                 seedStore + "/kleeprev", seedError)) {
          if (entry.is_regular_file(seedError)) {
            haveOwnSeeds = true;
            break;
          }
        }
        if (haveSeeds || haveOwnSeeds) {
          seedFlag = std::string(haveSeeds ? " --seed-dir='" + seedStore +
                                                 "/ktest'"
                                           : "") +
                     (haveOwnSeeds ? " --seed-dir='" + seedStore + "/kleeprev'"
                                   : "") +
                     " --allow-seed-extension --allow-seed-truncation"
                     " --seed-time=" +
                     std::to_string(std::max(
                         1u, static_cast<unsigned>(kleeBudget / 4))) +
                     "s";
        }
      }

      // Depth-first for Cover-Branches, and the reason is about what survives
      // the deadline rather than about search quality.
      //
      // KLEE's default search keeps thousands of states alive at once. Each
      // writes its .ktest only when it terminates, and the ones still live
      // when the budget expires are dumped at halt -- where writing them
      // fails wholesale: "unable to write output test case, losing it", 3982
      // times in one measured run, for a total of zero test cases from 3982
      // explored paths. Depth-first finishes states one after another, so
      // each one's test is on disk long before the deadline matters.
      //
      // For the property modes the default search stays: there the goal is to
      // find one violating path quickly, not to harvest many.
      std::string searchPolicy =
          (map2checkMode == Map2CheckMode::COVER_BRANCHES_MODE)
              ? " --search=dfs"
              : "";

      std::string stopPolicy =
          (map2checkMode == Map2CheckMode::COVER_BRANCHES_MODE)
              ? ""
              : " --exit-on-error-type=Abort";

      std::vector<std::string> kleebackendsolver = {"z3", "stp"};
      std::vector<std::string> kleemetasolver = {"btor", "yices2"};


      if ( std::count(kleebackendsolver.begin(), kleebackendsolver.end(), solvername) ) {
        // Checkout solver adopted, if is z3 or stp
        // in KLEE add -solver-backend option

        Map2Check::Log::Info("Solver backend caller: " + solvername);
        //  --allow-external-sym-calls
        //  -use-cache
        kleeCommand << " --external-calls=all"
                    << stopPolicy << searchPolicy << seedFlag
                    << " --optimize"
                    << " --use-cex-cache"
                    << " --solver-backend=" + solvername + " "
                    << " --libc=uclibc"
                    << " ./" + programHash + "-witness-result.bc"
                    << "  > ExecutionOutput.log";
      } else if ( std::count(kleemetasolver.begin(), kleemetasolver.end(), solvername) ) {
        // Checkout solver adopted, if is btor (Boolector) or yices (Yices)
        // in KLEE add - option
        Map2Check::Log::Info("Solver metaSMT caller: " + solvername);

        kleeCommand << " --external-calls=all"
                    << stopPolicy << searchPolicy << seedFlag
                    << " --optimize"
                    << " --use-cex-cache"
                    << " --solver-backend=metasmt "
                    << " --metasmt-backend=" + solvername + " "
                    << " --libc=uclibc"
                    << " ./" + programHash + "-witness-result.bc"
                    << "  > ExecutionOutput.log";
      }

      Map2Check::Log::Debug(kleeCommand.str());
      const auto engineStarted = std::chrono::steady_clock::now();
      int result = runKleeWatched(kleeCommand.str());
      if (this->engineSeconds != nullptr) {
        *this->engineSeconds = std::chrono::duration<double>(
                                   std::chrono::steady_clock::now() -
                                   engineStarted)
                                   .count();
      }
      if (this->seedExchange) {
        if (this->stagnationLimit > 0) keepKleeTestsAsSeeds();
        // Written after KLEE rather than before the next phase, because the
        // .ktest files are in the scratch directory that cleanGarbage() will
        // remove -- and because a later alternation, or a resumed run, should
        // find them already there.
        exportKleeVectorsAsSeeds();
      }
      Map2Check::Log::Warning("Exited klee with " + std::to_string(result));
      // KLEE found nothing: its vectors, run on natively. Not for
      // Cover-Branches, whose goal is no violation.
      if (map2checkMode != Map2CheckMode::COVER_BRANCHES_MODE &&
          !isWitnessFileCreated() &&
          !Map2Check::hasViolatingKtest(Map2Check::kleeOutputDir)) {
        replayKleeVectorsForViolation();
      }
      if (result == 31744)  // Timeout
        gotTimeout = true;
      // Stopped by us for want of progress: states were left, nothing proved.
      if (this->stoppedOnStagnation) gotTimeout = true;
      // KLEE exits 0 when its queue empties, like a run that explored every
      // path -- also after its timer, a concretized input or states it killed
      // itself. None of those proves anything. Treated as the timeout it is: a
      // violation already recorded is kept, anything else is UNKNOWN -- never
      // the TRUE a short path's NONE in the property file would otherwise make
      // it (a reachable null dereference, a float bug, came back TRUE).
      const std::string dropped =
          Map2Check::kleeDroppedPaths(Map2Check::kleeOutputDir);
      if (!dropped.empty()) {
        Map2Check::Log::Warning("KLEE " + dropped +
                                " -- not a complete exploration");
        gotTimeout = true;
      }

      break;
    }
    case (NonDetGenerator::AFLPlusPlus): {
      std::error_code fuzzErr;
      const bool hasFuzzer =
          std::filesystem::exists(programHash + "-fuzzed.out", fuzzErr);
      if (fuzzErr) {
        Map2Check::Log::Warning(
            "could not check whether the AFL++ binary is available: " +
            fuzzErr.message());
        break;
      }
      if (!hasFuzzer) {
        Map2Check::Log::Warning(
            "the AFL++ binary is unavailable -- skipping the fuzzer phase");
        break;
      }
      Map2Check::Log::Info("Executing AFL++ with map2check");
      std::ostringstream command;
      command.str("");
      // Against what is LEFT, not against the nominal budget -- see
      // Caller::remainingSeconds.
      // Alternating, the last window can be everything left: keep the same
      // reserve KLEE keeps for replaying crashes and writing the suite.
      const double fuzzerBudget =
          this->engineWindow > 0
              ? std::max(1.0,
                         std::min(this->engineWindow,
                                  static_cast<double>(this->remainingSeconds()) -
                                      5.0))
              : std::min(0.2 * this->timeout,
                         static_cast<double>(this->remainingSeconds()));
      // afl-fuzz needs a non-empty -i dir and a -o dir that does not already
      // exist (the hybrid may run the fuzzer phase twice).
      //
      // The input dir is the shared seed corpus only under --seed-exchange,
      // as it was for the previous fuzzer; otherwise a private one, so a run
      // without the exchange leaves no seeds/ behind. Either way it gets one
      // placeholder input when empty, because afl-fuzz refuses to start
      // without one (the previous fuzzer could start from nothing).
      std::error_code seedErr;
      const std::string inputDir =
          this->seedExchange ? seedStore + "/afl" : std::string("afl-in");
      std::filesystem::create_directories(inputDir, seedErr);
      if (std::filesystem::is_empty(inputDir, seedErr)) {
        std::ofstream seed(inputDir + "/seed");
        seed << "A";
        if (!seed.good())
          Map2Check::Log::Warning("could not write the AFL++ placeholder seed");
      }
      if (seedErr)
        Map2Check::Log::Warning("could not prepare the AFL++ input dir " +
                                inputDir + ": " + seedErr.message());
      std::filesystem::remove_all("afl-out", seedErr);
      // The AFL_* settings go on the command line, not only into the dev
      // image's ENV, so that a release install or a benchmark host outside
      // the image does not trip afl-fuzz's UI, CPU-affinity, cpufreq and
      // core_pattern checks and exit before fuzzing anything.
      //   AFL_CRASHING_SEEDS_AS_NEW_CRASH: a seed that already reaches the
      //     violation is recorded as a crash instead of being skipped -- the
      //     previous fuzzer reported that case too.
      //   AFL_BENCH_UNTIL_CRASH: stop at the first crash, as the previous
      //     fuzzer did, and hand the rest of the budget back.
      command << "AFL_NO_UI=1 AFL_NO_AFFINITY=1 AFL_SKIP_CPUFREQ=1"
              << " AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1"
              << " AFL_CRASHING_SEEDS_AS_NEW_CRASH=1"
              << " AFL_BENCH_UNTIL_CRASH=1 ";
      // Alternating: the fuzzer hands the rest of its window back as soon as
      // it stops finding coverage, and exits 0 doing so -- not a timeout.
      if (this->stagnationLimit > 0) {
        command << "AFL_EXIT_ON_TIME=" << this->stagnationLimit << " ";
      }
      // Bounded by `timeout` alone, as the previous fuzzer was. Not also by
      // afl-fuzz -V: that one compares wall-clock (gettimeofday) readings,
      // and a clock stepped backwards -- measured at over a second under
      // WSL2 -- underflows the difference and ends the run after a few
      // hundred executions. `timeout` uses a relative timer. At least 1s,
      // since `timeout 0` would mean no limit at all.
      command << "timeout -k " << Map2Check::killGracePeriod << " "
              << std::max(1u, static_cast<unsigned>(fuzzerBudget)) << " ";
      std::error_code cmplogErr;
      const bool hasCmplog =
          std::filesystem::exists(programHash + "-cmplog.out", cmplogErr);
      command << Map2Check::aflFuzzBinary()
              << " -i '" << inputDir << "'"
              << " -o afl-out";
      if (hasCmplog) command << " -c ./" << programHash << "-cmplog.out";
      command << " -- ./" << programHash << "-fuzzed.out"
              << " > fuzzer.output 2>&1";

      const auto engineStarted = std::chrono::steady_clock::now();
      int result = system(command.str().c_str());
      if (this->engineSeconds != nullptr) {
        *this->engineSeconds = std::chrono::duration<double>(
                                   std::chrono::steady_clock::now() -
                                   engineStarted)
                                   .count();
      }
      Map2Check::Log::Warning("Exited fuzzer with " + std::to_string(result));
      if (result == 31744)  // Timeout
        gotTimeout = true;

      // A single instance without -M/-S is named "default" by afl-fuzz, and
      // its findings live under afl-out/default/, not afl-out/.
      const std::string aflFindings = "afl-out/default";

      // Replay crashes with the witness binary to confirm a real violation.
      // Standalone, the persistent binary reads its input from stdin (see
      // NonDetGeneratorAFL.c), so the crash file is redirected in; the names
      // AFL++ gives them contain ':' and ',', hence the quoting. Stop at the
      // first confirmed one: every replay rewrites the recorded property, and
      // a later one that does not reproduce would overwrite the violation.
      // Each replay is capped: a crash that does not reproduce from a fresh
      // process may loop instead, and must not eat what is left for KLEE.
      // Files are selected by what they are, not by AFL++'s "id:" naming,
      // which AFL_SHA1_FILENAMES or a SIMPLE_FILES build would change.
      const unsigned replayBudget = std::min(
          this->remainingSeconds(),
          std::max(5u, static_cast<unsigned>(0.1 * this->timeout)));
      std::error_code crashErr;
      for (const auto &entry : std::filesystem::directory_iterator(
               aflFindings + "/crashes", crashErr)) {
        if (!entry.is_regular_file(crashErr)) continue;
        if (entry.path().filename() == "README.txt") continue;
        std::ostringstream commandWitness;
        commandWitness << "timeout -k " << Map2Check::killGracePeriod << " "
                       << replayBudget << " ./" << programHash
                       << "-witness-fuzzed.out < '" << entry.path().string()
                       << "'";
        system(commandWitness.str().c_str());
        if (isWitnessFileCreated()) break;
      }

      // afl-fuzz never writes back into -i, so under --seed-exchange its
      // discoveries are copied into seeds/ -- the previous fuzzer grew that
      // directory in place. Inputs tagged ",orig:" are the seeds it started
      // from, already there. The store is beside the scratch directory, so
      // the next fuzzer phase starts from this corpus plus KLEE's vectors.
      if (this->seedExchange) {
        std::error_code queueErr;
        for (const auto &entry : std::filesystem::directory_iterator(
                 aflFindings + "/queue", queueErr)) {
          const std::string name = entry.path().filename().string();
          if (!entry.is_regular_file(queueErr)) continue;
          if (name.find(",orig:") != std::string::npos) continue;
          std::filesystem::copy_file(
              entry.path(),
              seedStore + "/afl/afl-" + name,
              std::filesystem::copy_options::skip_existing, queueErr);
        }
      }
      // The fuzzer's corpus for the KLEE phase, unless this phase already
      // decided the property.
      if (this->seedExchange && this->feedsKleePhase &&
          !isWitnessFileCreated()) {
        exportFuzzerCorpusAsKtests();
      }
      Map2Check::Log::Debug("Finished fuzzer");

      if (isWitnessFileCreated()) {
        witnessVerified = true;
      }

      break;
    }
  }
  if (isWitnessFileCreated()) {
    witnessVerified = true;
  }
}

std::vector<int> Caller::processClangOutput() {
  const char* path_name = "clang.out";

  std::vector<int> result;

  ifstream in(path_name);
  if (!in.is_open()) {
    Map2Check::Log::Debug("Clang did not generate warning or errors");
    return result;
  }
  Map2Check::Log::Debug("Clang generate warning or errors");

  // This regex captures accused line number for overflow warnings (from clang)
  regex overflowWarning(
      ".*:([[:digit:]]+):[[:digit:]]+:.*(Winteger-overflow).*");
  string line;
  smatch match;
  while (getline(in, line)) {
    if (std::regex_search(line, match, overflowWarning) && match.size() > 1) {
      Map2Check::Log::Info("Found warning at line " + match[1].str());
      int lineNumber = std::stoi(match[1].str());
      result.push_back(lineNumber);
    }
  }

  return result;
}

/** This function should:
 * (1) Remove unsupported functions and clean the C code
 * (2) Generate .bc file from code
 * (3) Check for overflow errors on compilation
 */
void Caller::compileCFile(bool is_llvm_bc) {
  if (!is_llvm_bc) {
    Map2Check::Log::Info("Compiling " + this->pathprogram);

    // (1) Remove unsupported functions and clean the C code
    std::ostringstream commandRemoveExternMalloc;
    commandRemoveExternMalloc.str("");
    commandRemoveExternMalloc << "cat " << this->pathprogram << " | ";
    commandRemoveExternMalloc << "sed -e 's/extern void [*].[^_]*lloc.*/ / g' "
                              << " > " << programHash << "-preprocessed.c ";
    // Map2Check::Log::Info(commandRemoveExternMalloc.str().c_str());
    system(commandRemoveExternMalloc.str().c_str());

    std::ostringstream commandRemoveExternMemset;
    commandRemoveExternMemset.str("");
    commandRemoveExternMemset << "sed -i 's/extern void [*]memset.*/ / g' "
                              << " " << programHash << "-preprocessed.c ";
    // Map2Check::Log::Info(commandRemoveExternMemset.str().c_str());
    system(commandRemoveExternMemset.str().c_str());

    std::ostringstream commandRemoveVoidMemset;
    commandRemoveVoidMemset.str("");
    commandRemoveVoidMemset << "sed -i 's/void [*]memset(void[*], int, size_t);/ / g' "
                              << " " << programHash << "-preprocessed.c ";
    // Map2Check::Log::Info(commandRemoveExternMemset.str().c_str());
    system(commandRemoveVoidMemset.str().c_str());

    std::ostringstream commandRemoveVoidMemcpy;
    commandRemoveVoidMemcpy.str("");
    commandRemoveVoidMemcpy << "sed -i 's/void [*]memcpy(void[*], const void [*], size_t);/ / g' "
                              << " " << programHash << "-preprocessed.c ";
    // Map2Check::Log::Info(commandRemoveExternMemset.str().c_str());
    system(commandRemoveVoidMemcpy.str().c_str());

    // (2) Generate .bc file from code
    // TODO(hbgit): -Winteger-overflow should be called only if is on overflow
    // mode
    std::string compiledFile = programHash + "-compiled.bc";
    std::ostringstream command;
    command.str("");
    command << Map2Check::clangBinary << " -I" << Map2Check::clangIncludeFolder
            << " -Wno-everything "
            << " -Winteger-overflow "
            << " -c -emit-llvm -g"
            << " " << Caller::preOptimizationFlags() << " -o " << compiledFile
            << " " << programHash << "-preprocessed.c "
            << " > " << programHash << "-clang.out 2>&1";

    system(command.str().c_str());

    this->pathprogram = compiledFile;
  } else {
    std::string compiledFile = programHash + "-compiled.bc";
    std::ostringstream command;
    command.str("");
    command << " cp " << this->pathprogram << " " << compiledFile;
    system(command.str().c_str());
    this->pathprogram = compiledFile;
  }

  // TODO(hbgit): (3) Check for overflow errors on compilation
}

void Caller::compileWithClam() {
  Map2Check::Log::Info("Compiling with Clam (invariants) in " + this->pathprogram);

  // (1) Remove unsupported functions and clean the C code
  // TODO(hbgit): improve regex to the next line
  std::ostringstream commandRemoveExternMalloc;
  commandRemoveExternMalloc.str("");
  commandRemoveExternMalloc << "cat " << this->pathprogram << " | ";
  commandRemoveExternMalloc << "sed -e 's/extern void [*].[^_]*lloc.*/ / g' "
                            << " > " << programHash << "-preprocessed.c ";
  system(commandRemoveExternMalloc.str().c_str());

  std::ostringstream commandRemoveExternMemset;
  commandRemoveExternMemset.str("");
  commandRemoveExternMemset << "sed -i 's/extern void [*]memset.*/ / g' "
                            << " " << programHash << "-preprocessed.c ";
  // Map2Check::Log::Info(commandRemoveExternMemset.str().c_str());
  system(commandRemoveExternMemset.str().c_str());

  std::ostringstream commandRemoveVoidMemset;
  commandRemoveVoidMemset.str("");
  commandRemoveVoidMemset << "sed -i 's/void [*]memset(void[*], int, size_t);/ / g' "
                            << " " << programHash << "-preprocessed.c ";
  // Map2Check::Log::Info(commandRemoveExternMemset.str().c_str());
  system(commandRemoveVoidMemset.str().c_str());

  std::ostringstream commandRemoveVoidMemcpy;
  commandRemoveVoidMemcpy.str("");
  commandRemoveVoidMemcpy << "sed -i 's/void [*]memcpy(void[*], const void [*], size_t);/ / g' "
                            << " " << programHash << "-preprocessed.c ";
  // Map2Check::Log::Info(commandRemoveExternMemset.str().c_str());
  system(commandRemoveVoidMemcpy.str().c_str());


  // (2) Generate .bc file from code
  // TODO(hbgit): -Winteger-overflow should be called only if is on overflow
  // mode CLANG PATH
  std::ostringstream getPathCLCommand;
  getPathCLCommand.str("");
  std::ostringstream getMapPath;
  getMapPath << getenv("MAP2CHECK_PATH");

  getPathCLCommand << "CLANG_PATH=" << getMapPath.str().c_str() << "/bin";

  std::string tmp_gpcc = getPathCLCommand.str().c_str();
  char* c_gpcc = new char[tmp_gpcc.length() + 1];
  std::copy(tmp_gpcc.c_str(), tmp_gpcc.c_str() + tmp_gpcc.length() + 1, c_gpcc);
  putenv(c_gpcc);

  // Clam's shared libraries live under its own installation root, not under
  // the Map2Check prefix: it is an independent tool with its own LLVM-versioned
  // build. The old path pointed at ${MAP2CHECK_PATH}/bin/crabllvm/lib, which
  // has not existed since the LLVM 6 era.
  const char* clamDirEnv = getenv("CLAM_DIR");
  std::string clamRoot =
      clamDirEnv != nullptr ? std::string(clamDirEnv)
                            : std::string(Map2Check::clamDefaultRoot);

  const char* ldPathEnv = getenv("LD_LIBRARY_PATH");
  std::ostringstream getPathLibClamCommand;
  getPathLibClamCommand << "LD_LIBRARY_PATH="
                        << (ldPathEnv != nullptr ? ldPathEnv : "") << ":"
                        << clamRoot << "/lib";

  std::string tmp_gplibcc = getPathLibClamCommand.str();
  char* c_gplibcc = new char[tmp_gplibcc.length() + 1];
  std::copy(tmp_gplibcc.c_str(), tmp_gplibcc.c_str() + tmp_gplibcc.length() + 1,
            c_gplibcc);
  putenv(c_gplibcc);

  std::string compiledFile = programHash + "-compiled.bc";
  std::ostringstream command;
  command.str("");

  // This flag list is measured, not translated. Clam renamed one option
  //   crab-llvm: --crab-add-invariants=block-entry
  //   Clam:      --crab-opt=add-invariants --crab-opt-invariants-loc=block-entry
  // but two others in the legacy call actively defeat the purpose, and were
  // dropped after counting verifier.assume calls in the emitted bitcode on a
  // loop program (5 injected by the baseline flags below):
  //
  //   --llvm-pp-loops        5 -> 0. Its loop preprocessing runs after the
  //                          invariants would be placed, and nothing survives.
  //   --crab-promote-assume  5 -> 1. It rewrites verifier.assume into the
  //                          llvm.assume intrinsic (lib/Transforms/PromoteAssume.cc),
  //                          which is precisely the symbol NonDetPass.cpp:95
  //                          matches on -- promoting makes the invariants
  //                          invisible to Map2Check.
  //
  // So the legacy invocation would have injected nothing even if crab-llvm had
  // still built: the capability was dead twice over. --crab-inter and -m 64 -g
  // were verified harmless (still 5). --crab-disable-warnings and
  // --disable-lower-gv no longer exist in Clam's option set.
  //
  // The emitted verifier.assume is rewritten by NonDetPass into
  // map2check_crab_assume, which the runtime forwards to klee_assume -- so
  // nothing downstream needs to change.
  // See docs/reports/2026-08-16-crabllvm-review.md.
  command << Map2Check::clamBinary() << " -o " << compiledFile << " -m 64 -g"
          << " --crab-inter"
          << " --crab-track=num"
          << " --crab-opt=add-invariants"
          << " --crab-opt-invariants-loc=block-entry"
          << " " << programHash << "-preprocessed.c ";

  Map2Check::Log::Debug(command.str());
  system(command.str().c_str());

  this->pathprogram = compiledFile;
}

}  // namespace Map2Check
