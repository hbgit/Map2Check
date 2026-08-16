# Test-Comp Test-Suite Emitter Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make Map2Check emit a Test-Comp test suite (`metadata.xml` + one `*.xml`
per test case) reproducing the violation it found, so that a Cover-Error run produces
the artifact the competition actually scores.

**Architecture:** The runtime already records every nondeterministic value in
consumption order and flushes it to `klee_log.csv` (`NonDetLog.c:17`), for both the
KLEE and the LibFuzzer generator. Nothing new is instrumented. A new frontend module
reads that CSV and serializes it as Test-Comp XML, because only the frontend knows the
source path, the property, the architecture and the producer version. The suite is
written as a directory (what the competition consumes); zipping is the validator
harness's job, not the tool's.

**Tech Stack:** C++17, CMake + Ninja, GTest, Boost.ProgramOptions (existing stack; no
new third-party dependency).

**Spec:** [Test-Comp exchange format](https://gitlab.com/sosy-lab/software/test-format)
— `test-metadata.dtd`, `testcase.dtd`, `doc/Format.md`. Verbatim copies of both DTDs
are reproduced in the Global Constraints below so the implementer needs no network.

## Global Constraints

- **`test-metadata.dtd` (verbatim, upstream):**
  ```
  <!ELEMENT test-metadata (sourcecodelang, producer, specification, programfile, programhash, entryfunction, architecture, inputtestsuitefile*, inputtestsuitehash*, creationtime)>
  ```
  The element order above is **mandatory** — it is a DTD sequence, not a choice.
  `inputtestsuitefile` / `inputtestsuitehash` are optional and Map2Check does not emit
  them (they exist for tools that consume an existing suite).
- **`testcase.dtd` (verbatim, upstream):**
  ```
  <!ELEMENT testcase (input*)>
  <!ELEMENT input (#PCDATA)>
  <!ATTLIST testcase coversError (true|false) "false">
  <!ATTLIST input type CDATA "" variable CDATA "">
  ```
- **DOCTYPE lines (exact strings, from the upstream examples):**
  - metadata: `<!DOCTYPE test-metadata PUBLIC "+//IDN sosy-lab.org//DTD test-format test-metadata 1.1//EN" "https://sosy-lab.org/test-format/test-metadata-1.1.dtd">`
  - testcase: `<!DOCTYPE testcase PUBLIC "+//IDN sosy-lab.org//DTD test-format testcase 1.1//EN" "https://sosy-lab.org/test-format/testcase-1.1.dtd">`
- **`<programhash>` is SHA-256** of the program file. Map2Check currently has only
  SHA-1 (`gen_crypto_hash.hpp`, Boost); Task 1 adds SHA-256.
- **Input ordering is the whole contract.** The *n*-th `<input>` is the value returned
  by the *n*-th `__VERIFIER_nondet_*` call at runtime. `klee_log.csv` is already
  written in exactly that order, one row per call.
- **No `variable` attribute, and no `type` attribute, in this plan.** `doc/Format.md`
  states that if a `variable` is specified and the input sequence does not fit it,
  "the interpretation is undefined". Bare `<input>value</input>` is the form used by
  upstream `examples/testcase-2.xml` and carries no such risk.
- **Directory, not zip.** The tool writes `test-suite/`. TestCov's `--test-suite`
  takes a zip; the harness in the companion plan zips it.
- **Suite location must survive cleanup.** `Caller` chdirs into
  `<sha1>.map2check/` (`caller.cpp:75-76`) and `cleanGarbage()` deletes it
  (`caller.cpp:96-99`). The suite must be written under the *original* working
  directory.
- **Do not touch `--generate-testcase`.** That existing flag writes an annotated C
  program and is documented as experimental. The new flag is `--generate-test-suite`.
- C++ code follows Google style (`.clang-format`); run `./check_code_style.py -p`.

---

### Task 1: SHA-256 for the program hash

**Files:**
- Create: `modules/frontend/utils/sha256.hpp`
- Create: `modules/frontend/utils/sha256.cpp`
- Modify: `modules/frontend/utils/CMakeLists.txt`
- Create: `tests/unit/frontend/CMakeLists.txt`
- Modify: `tests/unit/CMakeLists.txt:5` (add `add_subdirectory(frontend)`)
- Test: `tests/unit/frontend/Sha256Test.cpp`

**Interfaces:**
- Consumes: nothing.
- Produces: `std::string Map2Check::sha256File(const std::string& path)` returning 64
  lowercase hex characters, and `std::string Map2Check::sha256Hex(const std::string&)`
  for the in-memory case. Task 2 uses `sha256File`.

Rationale for a self-contained implementation rather than a dependency: OpenSSL is not
currently linked, and shelling out to `sha256sum` would make a metadata field depend on
an external binary being present. SHA-256 is ~120 lines and is exactly testable
against published vectors.

- [ ] **Step 1: Write the failing test**

```cpp
// tests/unit/frontend/Sha256Test.cpp
#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>

#include "../../../modules/frontend/utils/sha256.hpp"

// Vectors from FIPS 180-4 / RFC 6234.
TEST(Sha256Test, EmptyString) {
  EXPECT_EQ(Map2Check::sha256Hex(""),
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

TEST(Sha256Test, Abc) {
  EXPECT_EQ(Map2Check::sha256Hex("abc"),
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

// 448 bits: forces a two-block message with the length spilling past the
// first padding block, which is where naive implementations break.
TEST(Sha256Test, TwoBlockMessage) {
  EXPECT_EQ(Map2Check::sha256Hex(
                "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
            "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
}

TEST(Sha256Test, OneMillionA) {
  EXPECT_EQ(Map2Check::sha256Hex(std::string(1000000, 'a')),
            "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
}

TEST(Sha256Test, HashesFileContentsNotPath) {
  std::string path = "sha256_fixture.txt";
  {
    std::ofstream out(path, std::ios::binary);
    out << "abc";
  }
  EXPECT_EQ(Map2Check::sha256File(path),
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
  std::remove(path.c_str());
}

TEST(Sha256Test, MissingFileReturnsEmptyString) {
  EXPECT_EQ(Map2Check::sha256File("no_such_file_here.bin"), "");
}
```

- [ ] **Step 2: Run it and confirm it fails to compile**

```bash
cd build && cmake .. -G Ninja -DLLVM_DIR=/usr/lib/llvm-16/lib/cmake/llvm \
  -DSKIP_LIB_FUZZER=ON -DSKIP_KLEE=ON -DENABLE_TEST=ON && ninja Sha256Test
```
Expected: FAIL — `sha256.hpp: No such file or directory`.

- [ ] **Step 3: Write the header**

```cpp
// modules/frontend/utils/sha256.hpp
/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#ifndef MODULES_FRONTEND_UTILS_SHA256_HPP_
#define MODULES_FRONTEND_UTILS_SHA256_HPP_

#include <string>

namespace Map2Check {

/** SHA-256 of an in-memory buffer, as 64 lowercase hex characters. */
std::string sha256Hex(const std::string& data);

/** SHA-256 of a file's contents. Returns "" if the file cannot be read.
 * The Test-Comp <programhash> field is defined over the program file, so this
 * hashes contents and never the path. */
std::string sha256File(const std::string& path);

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_UTILS_SHA256_HPP_
```

- [ ] **Step 4: Write the implementation**

```cpp
// modules/frontend/utils/sha256.cpp
/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include "sha256.hpp"

#include <array>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <vector>

namespace Map2Check {
namespace {

constexpr std::array<uint32_t, 64> kRoundConstants = {
    0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu,
    0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u, 0xd807aa98u, 0x12835b01u,
    0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u,
    0xc19bf174u, 0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu,
    0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau, 0x983e5152u,
    0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u,
    0x06ca6351u, 0x14292967u, 0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu,
    0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
    0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u,
    0xd6990624u, 0xf40e3585u, 0x106aa070u, 0x19a4c116u, 0x1e376c08u,
    0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu,
    0x682e6ff3u, 0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
    0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u};

inline uint32_t rotr(uint32_t x, unsigned n) {
  return (x >> n) | (x << (32u - n));
}

void compress(std::array<uint32_t, 8>* state, const uint8_t block[64]) {
  std::array<uint32_t, 64> w{};
  for (unsigned i = 0; i < 16; i++) {
    w[i] = (static_cast<uint32_t>(block[i * 4]) << 24) |
           (static_cast<uint32_t>(block[i * 4 + 1]) << 16) |
           (static_cast<uint32_t>(block[i * 4 + 2]) << 8) |
           static_cast<uint32_t>(block[i * 4 + 3]);
  }
  for (unsigned i = 16; i < 64; i++) {
    uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
    uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
    w[i] = w[i - 16] + s0 + w[i - 7] + s1;
  }

  std::array<uint32_t, 8> v = *state;
  for (unsigned i = 0; i < 64; i++) {
    uint32_t s1 = rotr(v[4], 6) ^ rotr(v[4], 11) ^ rotr(v[4], 25);
    uint32_t ch = (v[4] & v[5]) ^ (~v[4] & v[6]);
    uint32_t t1 = v[7] + s1 + ch + kRoundConstants[i] + w[i];
    uint32_t s0 = rotr(v[0], 2) ^ rotr(v[0], 13) ^ rotr(v[0], 22);
    uint32_t maj = (v[0] & v[1]) ^ (v[0] & v[2]) ^ (v[1] & v[2]);
    uint32_t t2 = s0 + maj;

    v[7] = v[6]; v[6] = v[5]; v[5] = v[4]; v[4] = v[3] + t1;
    v[3] = v[2]; v[2] = v[1]; v[1] = v[0]; v[0] = t1 + t2;
  }
  for (unsigned i = 0; i < 8; i++) (*state)[i] += v[i];
}

}  // namespace

std::string sha256Hex(const std::string& data) {
  std::array<uint32_t, 8> state = {0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u,
                                   0xa54ff53au, 0x510e527fu, 0x9b05688cu,
                                   0x1f83d9abu, 0x5be0cd19u};

  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data.data());
  size_t len = data.size();
  size_t full = len / 64;
  for (size_t i = 0; i < full; i++) compress(&state, bytes + i * 64);

  // Tail: remaining bytes, 0x80, zero padding, then the 64-bit bit length.
  // Two blocks are needed when the remainder leaves no room for the length.
  std::vector<uint8_t> tail(bytes + full * 64, bytes + len);
  tail.push_back(0x80);
  while (tail.size() % 64 != 56) tail.push_back(0x00);
  uint64_t bitLength = static_cast<uint64_t>(len) * 8u;
  for (int i = 7; i >= 0; i--) {
    tail.push_back(static_cast<uint8_t>((bitLength >> (i * 8)) & 0xffu));
  }
  for (size_t i = 0; i < tail.size(); i += 64) compress(&state, tail.data() + i);

  std::ostringstream out;
  for (uint32_t word : state) {
    for (int i = 3; i >= 0; i--) {
      unsigned byte = (word >> (i * 8)) & 0xffu;
      out << "0123456789abcdef"[byte >> 4] << "0123456789abcdef"[byte & 0xf];
    }
  }
  return out.str();
}

std::string sha256File(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in.is_open()) return "";
  std::ostringstream buffer;
  buffer << in.rdbuf();
  return sha256Hex(buffer.str());
}

}  // namespace Map2Check
```

- [ ] **Step 5: Wire it into CMake**

Append to `modules/frontend/utils/CMakeLists.txt`:

```cmake
add_library(Sha256 OBJECT sha256.cpp)
set_target_properties(Sha256 PROPERTIES COMPILE_FLAGS ${CPP_FLAGS})
```

Create `tests/unit/frontend/CMakeLists.txt`:

```cmake
add_executable(Sha256Test
    Sha256Test.cpp
    $<TARGET_OBJECTS:Sha256>
)
map2check_test(Sha256Test)
```

Add to `tests/unit/CMakeLists.txt`, after `add_subdirectory(backend-library)`:

```cmake
add_subdirectory(frontend)
```

- [ ] **Step 6: Run the tests and verify they pass**

```bash
cd build && ninja Sha256Test && ./tests/unit/frontend/Sha256Test
```
Expected: 6 tests, all PASS.

- [ ] **Step 7: Commit**

```bash
git add modules/frontend/utils/sha256.hpp modules/frontend/utils/sha256.cpp \
        modules/frontend/utils/CMakeLists.txt tests/unit/CMakeLists.txt \
        tests/unit/frontend/CMakeLists.txt tests/unit/frontend/Sha256Test.cpp
git commit -m "feat(utils): add SHA-256 for the Test-Comp programhash field"
```

---

### Task 2: The test-suite module

**Files:**
- Create: `modules/frontend/test_suite/test_suite.hpp`
- Create: `modules/frontend/test_suite/test_suite.cpp`
- Create: `modules/frontend/test_suite/CMakeLists.txt`
- Modify: `modules/frontend/CMakeLists.txt:4` (add `add_subdirectory(test_suite)`)
- Modify: `tests/unit/frontend/CMakeLists.txt`
- Test: `tests/unit/frontend/TestSuiteTest.cpp`

**Interfaces:**
- Consumes: `Map2Check::sha256File` from Task 1 (used by the caller, not here).
- Produces:
  - `struct Map2Check::TestSuiteMetadata` with fields `producer, specification,
    programFile, programHash, entryFunction, architecture, creationTime`
    (all `std::string`).
  - `std::vector<std::string> Map2Check::readNonDetLog(const std::string& csvPath)`
  - `class Map2Check::TestSuiteWriter` with
    `explicit TestSuiteWriter(std::string directory)`,
    `bool writeMetadata(const TestSuiteMetadata&)`,
    `bool writeTestCase(const std::vector<std::string>& inputs, bool coversError)`,
    `std::string isoUtcNow()` as a free function.
  - Task 3 calls all of these.

- [ ] **Step 1: Write the failing test**

```cpp
// tests/unit/frontend/TestSuiteTest.cpp
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "../../../modules/frontend/test_suite/test_suite.hpp"

namespace fs = std::filesystem;

namespace {

std::string slurp(const fs::path& p) {
  std::ifstream in(p);
  std::ostringstream buf;
  buf << in.rdbuf();
  return buf.str();
}

// Every test gets its own directory so nothing leaks between cases.
fs::path freshDir(const std::string& name) {
  fs::path d = fs::temp_directory_path() / ("m2c_ts_" + name);
  fs::remove_all(d);
  return d;
}

Map2Check::TestSuiteMetadata sampleMetadata() {
  Map2Check::TestSuiteMetadata md;
  md.producer = "Map2Check v7.3.1";
  md.specification = "COVER( init(main()), FQL(COVER EDGES(@CALL(reach_error))) )";
  md.programFile = "./digits.c";
  md.programHash = "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
  md.entryFunction = "main";
  md.architecture = "64bit";
  md.creationTime = "2026-08-16T12:00:00Z";
  return md;
}

}  // namespace

// --- metadata.xml -----------------------------------------------------------

TEST(TestSuiteWriter, MetadataHasDeclarationAndDoctypeFirst) {
  fs::path d = freshDir("meta_head");
  Map2Check::TestSuiteWriter w(d.string());
  ASSERT_TRUE(w.writeMetadata(sampleMetadata()));

  std::string xml = slurp(d / "metadata.xml");
  EXPECT_EQ(xml.find("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"), 0u);
  EXPECT_NE(xml.find("<!DOCTYPE test-metadata PUBLIC "
                     "\"+//IDN sosy-lab.org//DTD test-format test-metadata 1.1//EN\" "
                     "\"https://sosy-lab.org/test-format/test-metadata-1.1.dtd\">"),
            std::string::npos);
  fs::remove_all(d);
}

// The DTD declares a sequence, so a validator rejects reordered elements.
TEST(TestSuiteWriter, MetadataElementsFollowTheDtdOrder) {
  fs::path d = freshDir("meta_order");
  Map2Check::TestSuiteWriter w(d.string());
  ASSERT_TRUE(w.writeMetadata(sampleMetadata()));

  std::string xml = slurp(d / "metadata.xml");
  const std::vector<std::string> order = {
      "<sourcecodelang>", "<producer>",      "<specification>",
      "<programfile>",    "<programhash>",   "<entryfunction>",
      "<architecture>",   "<creationtime>"};
  size_t previous = 0;
  for (const std::string& tag : order) {
    size_t at = xml.find(tag);
    ASSERT_NE(at, std::string::npos) << "missing " << tag;
    EXPECT_GT(at, previous) << tag << " is out of order";
    previous = at;
  }
  fs::remove_all(d);
}

TEST(TestSuiteWriter, MetadataCarriesTheSuppliedValues) {
  fs::path d = freshDir("meta_values");
  Map2Check::TestSuiteWriter w(d.string());
  ASSERT_TRUE(w.writeMetadata(sampleMetadata()));

  std::string xml = slurp(d / "metadata.xml");
  EXPECT_NE(xml.find("<sourcecodelang>C</sourcecodelang>"), std::string::npos);
  EXPECT_NE(xml.find("<producer>Map2Check v7.3.1</producer>"), std::string::npos);
  EXPECT_NE(xml.find("<programfile>./digits.c</programfile>"), std::string::npos);
  EXPECT_NE(xml.find("<architecture>64bit</architecture>"), std::string::npos);
  EXPECT_NE(xml.find("<creationtime>2026-08-16T12:00:00Z</creationtime>"),
            std::string::npos);
  fs::remove_all(d);
}

// The specification string is FQL and contains no XML metacharacters today,
// but '&' and '<' in any field must never produce malformed XML.
TEST(TestSuiteWriter, MetadataEscapesXmlMetacharacters) {
  fs::path d = freshDir("meta_escape");
  Map2Check::TestSuiteMetadata md = sampleMetadata();
  md.programFile = "./a&b<c>.c";
  Map2Check::TestSuiteWriter w(d.string());
  ASSERT_TRUE(w.writeMetadata(md));

  std::string xml = slurp(d / "metadata.xml");
  EXPECT_NE(xml.find("<programfile>./a&amp;b&lt;c&gt;.c</programfile>"),
            std::string::npos);
  fs::remove_all(d);
}

// --- testcase files ---------------------------------------------------------

TEST(TestSuiteWriter, TestCaseHasDoctypeAndInputsInOrder) {
  fs::path d = freshDir("tc_order");
  Map2Check::TestSuiteWriter w(d.string());
  ASSERT_TRUE(w.writeTestCase({"1023", "-7", "254"}, true));

  std::string xml = slurp(d / "testcase-1.xml");
  EXPECT_EQ(xml.find("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"), 0u);
  EXPECT_NE(xml.find("<!DOCTYPE testcase PUBLIC "
                     "\"+//IDN sosy-lab.org//DTD test-format testcase 1.1//EN\" "
                     "\"https://sosy-lab.org/test-format/testcase-1.1.dtd\">"),
            std::string::npos);

  size_t first = xml.find("<input>1023</input>");
  size_t second = xml.find("<input>-7</input>");
  size_t third = xml.find("<input>254</input>");
  ASSERT_NE(first, std::string::npos);
  ASSERT_NE(second, std::string::npos);
  ASSERT_NE(third, std::string::npos);
  EXPECT_LT(first, second);
  EXPECT_LT(second, third);
  fs::remove_all(d);
}

TEST(TestSuiteWriter, CoversErrorAttributeReflectsTheArgument) {
  fs::path d = freshDir("tc_covers");
  Map2Check::TestSuiteWriter w(d.string());
  ASSERT_TRUE(w.writeTestCase({"1"}, true));
  ASSERT_TRUE(w.writeTestCase({"2"}, false));

  EXPECT_NE(slurp(d / "testcase-1.xml").find("<testcase coversError=\"true\">"),
            std::string::npos);
  EXPECT_NE(slurp(d / "testcase-2.xml").find("<testcase>"), std::string::npos);
  EXPECT_EQ(slurp(d / "testcase-2.xml").find("coversError"), std::string::npos);
  fs::remove_all(d);
}

TEST(TestSuiteWriter, TestCasesAreNumberedFromOne) {
  fs::path d = freshDir("tc_numbering");
  Map2Check::TestSuiteWriter w(d.string());
  ASSERT_TRUE(w.writeTestCase({"1"}, false));
  ASSERT_TRUE(w.writeTestCase({"2"}, false));
  ASSERT_TRUE(w.writeTestCase({"3"}, false));

  EXPECT_TRUE(fs::exists(d / "testcase-1.xml"));
  EXPECT_TRUE(fs::exists(d / "testcase-2.xml"));
  EXPECT_TRUE(fs::exists(d / "testcase-3.xml"));
  EXPECT_FALSE(fs::exists(d / "testcase-0.xml"));
  fs::remove_all(d);
}

// A program that consumes no nondeterministic input still produces a valid,
// empty test case -- dropping it would leave a suite with no test at all.
TEST(TestSuiteWriter, EmptyInputVectorStillWritesAValidTestCase) {
  fs::path d = freshDir("tc_empty");
  Map2Check::TestSuiteWriter w(d.string());
  ASSERT_TRUE(w.writeTestCase({}, true));

  std::string xml = slurp(d / "testcase-1.xml");
  EXPECT_NE(xml.find("<testcase coversError=\"true\">"), std::string::npos);
  EXPECT_NE(xml.find("</testcase>"), std::string::npos);
  EXPECT_EQ(xml.find("<input>"), std::string::npos);
  fs::remove_all(d);
}

TEST(TestSuiteWriter, CreatesTheDirectoryIfItIsAbsent) {
  fs::path d = freshDir("tc_mkdir") / "nested" / "test-suite";
  Map2Check::TestSuiteWriter w(d.string());
  ASSERT_TRUE(w.writeTestCase({"1"}, false));
  EXPECT_TRUE(fs::exists(d / "testcase-1.xml"));
  fs::remove_all(freshDir("tc_mkdir"));
}

// --- reading klee_log.csv ---------------------------------------------------

TEST(ReadNonDetLog, ReturnsValuesInConsumptionOrder) {
  fs::path d = freshDir("log_order");
  fs::create_directories(d);
  fs::path csv = d / "klee_log.csv";
  {
    // id;line;scope;function;step;value;type -- as written by NonDetLog.c
    std::ofstream out(csv);
    out << "0;12;0;main;1;1023;0\n";
    out << "1;13;0;main;2;-7;0\n";
    out << "2;14;0;main;3;254;5\n";
  }
  std::vector<std::string> values = Map2Check::readNonDetLog(csv.string());
  ASSERT_EQ(values.size(), 3u);
  EXPECT_EQ(values[0], "1023");
  EXPECT_EQ(values[1], "-7");
  EXPECT_EQ(values[2], "254");
  fs::remove_all(d);
}

TEST(ReadNonDetLog, SkipsMalformedRowsRatherThanThrowing) {
  fs::path d = freshDir("log_malformed");
  fs::create_directories(d);
  fs::path csv = d / "klee_log.csv";
  {
    std::ofstream out(csv);
    out << "0;12;0;main;1;1023;0\n";
    out << "garbage\n";
    out << "\n";
    out << "1;13;0;main;2;42;0\n";
  }
  std::vector<std::string> values = Map2Check::readNonDetLog(csv.string());
  ASSERT_EQ(values.size(), 2u);
  EXPECT_EQ(values[0], "1023");
  EXPECT_EQ(values[1], "42");
  fs::remove_all(d);
}

TEST(ReadNonDetLog, MissingFileYieldsAnEmptyVector) {
  EXPECT_TRUE(Map2Check::readNonDetLog("no_such_log.csv").empty());
}

// --- timestamp --------------------------------------------------------------

TEST(IsoUtcNow, MatchesTheFormatTheDtdConsumersExpect) {
  std::string now = Map2Check::isoUtcNow();
  ASSERT_EQ(now.size(), 20u);          // 2026-08-16T12:00:00Z
  EXPECT_EQ(now[4], '-');
  EXPECT_EQ(now[7], '-');
  EXPECT_EQ(now[10], 'T');
  EXPECT_EQ(now[13], ':');
  EXPECT_EQ(now[16], ':');
  EXPECT_EQ(now[19], 'Z');
}
```

- [ ] **Step 2: Run it and confirm it fails to compile**

```bash
cd build && ninja TestSuiteTest
```
Expected: FAIL — `test_suite/test_suite.hpp: No such file or directory`.

- [ ] **Step 3: Write the header**

```cpp
// modules/frontend/test_suite/test_suite.hpp
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
 * The format is defined by https://gitlab.com/sosy-lab/software/test-format.
 * A suite is a directory holding one metadata.xml plus one *.xml per test
 * case. Each test case is a sequence of <input> elements whose order is the
 * order in which the program consumed nondeterministic values.
 *
 * Map2Check already has that sequence: the runtime appends every
 * __VERIFIER_nondet_* call to an ordered log (NonDetLog.c) and flushes it to
 * klee_log.csv on exit, under both the KLEE and the LibFuzzer generator. This
 * module only serializes it -- nothing new is instrumented.
 ***********************************************************************/

#ifndef MODULES_FRONTEND_TEST_SUITE_TEST_SUITE_HPP_
#define MODULES_FRONTEND_TEST_SUITE_TEST_SUITE_HPP_

#include <string>
#include <vector>

namespace Map2Check {

/** Fields of metadata.xml. The DTD declares them as a sequence, so the writer
 * emits them in exactly this order. */
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
 * consumption order. Malformed rows are skipped: a partially written log
 * still yields a usable prefix, which is better than no test case at all.
 * Returns an empty vector if the file is absent. */
std::vector<std::string> readNonDetLog(const std::string& csvPath);

/** Writes a Test-Comp test suite into a directory, creating it if needed. */
class TestSuiteWriter {
 public:
  explicit TestSuiteWriter(std::string directory);

  /** Writes metadata.xml. Returns false if the file could not be written. */
  bool writeMetadata(const TestSuiteMetadata& metadata);

  /** Writes testcase-<n>.xml, numbering from 1 in call order. */
  bool writeTestCase(const std::vector<std::string>& inputs, bool coversError);

 private:
  std::string directory;
  unsigned counter;
};

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_TEST_SUITE_TEST_SUITE_HPP_
```

- [ ] **Step 4: Write the implementation**

```cpp
// modules/frontend/test_suite/test_suite.cpp
/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include "test_suite.hpp"

#include <ctime>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>

namespace Map2Check {
namespace {

const char kXmlDeclaration[] =
    "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>";

const char kMetadataDoctype[] =
    "<!DOCTYPE test-metadata PUBLIC "
    "\"+//IDN sosy-lab.org//DTD test-format test-metadata 1.1//EN\" "
    "\"https://sosy-lab.org/test-format/test-metadata-1.1.dtd\">";

const char kTestCaseDoctype[] =
    "<!DOCTYPE testcase PUBLIC "
    "\"+//IDN sosy-lab.org//DTD test-format testcase 1.1//EN\" "
    "\"https://sosy-lab.org/test-format/testcase-1.1.dtd\">";

/** Program paths and FQL specifications are attacker-free here, but a stray
 * '&' in a filename would still produce a document no validator accepts. */
std::string escapeXml(const std::string& raw) {
  std::string out;
  out.reserve(raw.size());
  for (char c : raw) {
    switch (c) {
      case '&': out += "&amp;"; break;
      case '<': out += "&lt;"; break;
      case '>': out += "&gt;"; break;
      case '"': out += "&quot;"; break;
      case '\'': out += "&apos;"; break;
      default: out += c;
    }
  }
  return out;
}

}  // namespace

std::string isoUtcNow() {
  std::time_t now = std::time(nullptr);
  std::tm utc{};
#ifdef _WIN32
  gmtime_s(&utc, &now);
#else
  gmtime_r(&now, &utc);
#endif
  char buffer[21];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &utc);
  return std::string(buffer);
}

std::vector<std::string> readNonDetLog(const std::string& csvPath) {
  std::vector<std::string> values;
  std::ifstream in(csvPath);
  if (!in.is_open()) return values;

  // Row layout written by NonDetLog.c:
  //   id;line;scope;function_name;step;value;type
  // The value is field 5 (zero-based).
  std::string line;
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    std::vector<std::string> fields;
    std::string field;
    std::istringstream row(line);
    while (std::getline(row, field, ';')) fields.push_back(field);
    if (fields.size() < 7) continue;
    values.push_back(fields[5]);
  }
  return values;
}

TestSuiteWriter::TestSuiteWriter(std::string directory)
    : directory(std::move(directory)), counter(0) {}

bool TestSuiteWriter::writeMetadata(const TestSuiteMetadata& metadata) {
  std::error_code ec;
  std::filesystem::create_directories(this->directory, ec);

  std::ofstream out(std::filesystem::path(this->directory) / "metadata.xml");
  if (!out.is_open()) return false;

  out << kXmlDeclaration << "\n"
      << kMetadataDoctype << "\n"
      << "<test-metadata>\n"
      << "  <sourcecodelang>C</sourcecodelang>\n"
      << "  <producer>" << escapeXml(metadata.producer) << "</producer>\n"
      << "  <specification>" << escapeXml(metadata.specification)
      << "</specification>\n"
      << "  <programfile>" << escapeXml(metadata.programFile)
      << "</programfile>\n"
      << "  <programhash>" << escapeXml(metadata.programHash)
      << "</programhash>\n"
      << "  <entryfunction>" << escapeXml(metadata.entryFunction)
      << "</entryfunction>\n"
      << "  <architecture>" << escapeXml(metadata.architecture)
      << "</architecture>\n"
      << "  <creationtime>" << escapeXml(metadata.creationTime)
      << "</creationtime>\n"
      << "</test-metadata>\n";
  return out.good();
}

bool TestSuiteWriter::writeTestCase(const std::vector<std::string>& inputs,
                                    bool coversError) {
  std::error_code ec;
  std::filesystem::create_directories(this->directory, ec);

  this->counter++;
  std::string name = "testcase-" + std::to_string(this->counter) + ".xml";
  std::ofstream out(std::filesystem::path(this->directory) / name);
  if (!out.is_open()) return false;

  out << kXmlDeclaration << "\n" << kTestCaseDoctype << "\n";
  // coversError defaults to "false" in the DTD, so it is only emitted when true.
  out << (coversError ? "<testcase coversError=\"true\">\n" : "<testcase>\n");
  for (const std::string& value : inputs) {
    out << "  <input>" << escapeXml(value) << "</input>\n";
  }
  out << "</testcase>\n";
  return out.good();
}

}  // namespace Map2Check
```

- [ ] **Step 5: Wire it into CMake**

Create `modules/frontend/test_suite/CMakeLists.txt`:

```cmake
add_library(TestSuite OBJECT test_suite.cpp)
set_target_properties(TestSuite PROPERTIES COMPILE_FLAGS ${CPP_FLAGS})
```

Add to `modules/frontend/CMakeLists.txt`, after `add_subdirectory(witness)`:

```cmake
add_subdirectory(test_suite)
```

Append to `tests/unit/frontend/CMakeLists.txt`:

```cmake
add_executable(TestSuiteTest
    TestSuiteTest.cpp
    $<TARGET_OBJECTS:TestSuite>
)
map2check_test(TestSuiteTest)
```

- [ ] **Step 6: Run the tests and verify they pass**

```bash
cd build && ninja TestSuiteTest && ./tests/unit/frontend/TestSuiteTest
```
Expected: 13 tests, all PASS.

- [ ] **Step 7: Commit**

```bash
git add modules/frontend/test_suite modules/frontend/CMakeLists.txt \
        tests/unit/frontend/CMakeLists.txt tests/unit/frontend/TestSuiteTest.cpp
git commit -m "feat(test-suite): serialize the nondet log as Test-Comp XML"
```

---

### Task 3: Wire the emitter into the pipeline

**Files:**
- Modify: `modules/frontend/caller.hpp:63` (add a public accessor)
- Modify: `modules/frontend/map2check.cpp` (arg struct, CLI options, emission site)
- Modify: `modules/frontend/CMakeLists.txt` (link the new objects into `map2check`)
- Test: `tests/integration/test_test_suite_emission.sh`
- Modify: `.github/workflows/ci.yml` (run the new integration test)

**Interfaces:**
- Consumes: `Map2Check::sha256File` (Task 1); `TestSuiteWriter`, `readNonDetLog`,
  `isoUtcNow`, `TestSuiteMetadata` (Task 2); `Map2Check::kleeLogCSV`
  (`utils/tools.hpp:55`).
- Produces: the CLI flags `--generate-test-suite`, `--test-suite-dir <path>`,
  `--property-file <path>`, `--architecture <32bit|64bit>`; and
  `std::string Caller::getOriginalPath()`.

Two facts constrain where the call goes. `Caller` chdirs into `<sha1>.map2check/`
(`caller.cpp:75-76`), so `klee_log.csv` is only reachable from inside that directory;
and `cleanGarbage()` deletes it (`caller.cpp:96-99`). Emission therefore happens at
the violation branch in `map2check.cpp` — after `executeAnalysis`, before
`cleanGarbage` — writing to a path under the original working directory.

- [ ] **Step 1: Write the failing integration test**

```bash
#!/bin/bash
# test_test_suite_emission.sh -- --generate-test-suite must produce a suite that
# matches the Test-Comp exchange format.
#
# This asserts structure, not coverage: whether the suite actually reproduces
# the bug is TestCov's question, and it is asked by the companion CI gate. What
# is checked here is the part that no other test covers -- that the files exist,
# are well-formed, carry the right DOCTYPEs, and that the input sequence is the
# sequence the program consumed.

set -u

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MAP2CHECK="${MAP2CHECK_PATH:-/workspace/install_e2e}/bin/map2check"

PASSED=0
FAILED=0
ok()   { echo "  PASS $1"; PASSED=$((PASSED+1)); }
fail() { echo "  FAIL $1: $2"; FAILED=$((FAILED+1)); }

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

# Two nondet reads, then an error reachable only when both match. The values in
# the suite must therefore be 42 and 7, in that order.
cat > "$WORK/target.c" <<'EOF'
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
int main(void) {
  int a = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();
  if (a == 42 && b == 7) {
    reach_error();
  }
  return 0;
}
EOF

echo "============================================================"
echo "Test-Comp test-suite emission"
echo "============================================================"

( cd "$WORK" && timeout -k 10 300 "$MAP2CHECK" --target-function \
    --target-function-name reach_error --generate-test-suite \
    --nondet-generator klee --timeout 120 target.c > run.log 2>&1 )

SUITE="$WORK/test-suite"

if [ -d "$SUITE" ]; then
  ok "test-suite/ directory created"
else
  fail "suite directory" "not created; see $WORK/run.log"
  echo "Results: $PASSED passed, $FAILED failed"; exit 1
fi

[ -f "$SUITE/metadata.xml" ] && ok "metadata.xml written" \
                             || fail "metadata.xml" "absent"

n_tc=$(find "$SUITE" -name 'testcase-*.xml' | wc -l)
[ "$n_tc" -ge 1 ] && ok "at least one testcase file ($n_tc)" \
                  || fail "testcase files" "none written"

# Well-formedness, checked by a real parser rather than by grepping.
if command -v xmllint >/dev/null 2>&1; then
  bad=0
  for f in "$SUITE"/*.xml; do
    xmllint --noout --nonet "$f" 2>/dev/null || { bad=1; echo "    bad: $f"; }
  done
  [ "$bad" -eq 0 ] && ok "every emitted file is well-formed XML" \
                   || fail "well-formedness" "xmllint rejected a file"
else
  fail "well-formedness" "xmllint not installed"
fi

grep -q 'DTD test-format test-metadata 1.1//EN' "$SUITE/metadata.xml" \
  && ok "metadata carries the 1.1 public identifier" \
  || fail "metadata doctype" "public identifier missing or wrong"

grep -q '<sourcecodelang>C</sourcecodelang>' "$SUITE/metadata.xml" \
  && ok "sourcecodelang is C" || fail "sourcecodelang" "wrong or missing"

grep -qE '<programhash>[0-9a-f]{64}</programhash>' "$SUITE/metadata.xml" \
  && ok "programhash is 64 hex characters (SHA-256)" \
  || fail "programhash" "not a SHA-256 digest"

# The hash must be of the program, so it has to match sha256sum exactly.
want=$(sha256sum "$WORK/target.c" | cut -d' ' -f1)
got=$(sed -n 's:.*<programhash>\(.*\)</programhash>.*:\1:p' "$SUITE/metadata.xml")
[ "$want" = "$got" ] && ok "programhash matches sha256sum of the program" \
                     || fail "programhash value" "want $want, got $got"

first_tc=$(find "$SUITE" -name 'testcase-*.xml' | sort | head -1)

grep -q 'DTD test-format testcase 1.1//EN' "$first_tc" \
  && ok "testcase carries the 1.1 public identifier" \
  || fail "testcase doctype" "public identifier missing or wrong"

grep -q 'coversError="true"' "$first_tc" \
  && ok "the violating test case is marked coversError" \
  || fail "coversError" "not set on a run that found the error"

# The ordered input vector is the whole point of the format.
mapfile -t inputs < <(sed -n 's:.*<input>\(.*\)</input>.*:\1:p' "$first_tc")
if [ "${#inputs[@]}" -eq 2 ] && [ "${inputs[0]}" = "42" ] && [ "${inputs[1]}" = "7" ]; then
  ok "inputs are 42 then 7, in consumption order"
else
  fail "input vector" "expected [42, 7], got [${inputs[*]}]"
fi

# The suite must outlive cleanGarbage(), which removes the scratch directory.
[ -z "$(find "$WORK" -maxdepth 1 -name '*.map2check' -print -quit)" ] \
  && ok "scratch directory cleaned and suite survived" \
  || fail "cleanup" "scratch directory left behind"

echo "============================================================"
echo "Results: $PASSED passed, $FAILED failed"
echo "============================================================"
[ "$FAILED" -eq 0 ] && exit 0 || exit 1
```

- [ ] **Step 2: Run it and verify it fails**

```bash
bash tests/integration/test_test_suite_emission.sh
```
Expected: FAIL at "test-suite/ directory created" — `--generate-test-suite` is not a
recognised option yet, so the run aborts.

- [ ] **Step 3: Expose the original working directory on Caller**

In `modules/frontend/caller.hpp`, in the `public:` section (after line 106):

```cpp
  /** Directory map2check was invoked from. The pipeline chdirs into a scratch
   * directory that cleanGarbage() later deletes, so anything meant to outlive
   * the run -- the Test-Comp suite, in particular -- must be written here. */
  std::string getOriginalPath() { return currentPath; }
```

- [ ] **Step 4: Add the CLI options**

In `modules/frontend/map2check.cpp`, add to the arguments struct next to
`bool invCrabLlvm = false;` (line 152):

```cpp
  bool generateTestSuite = false;
  std::string testSuiteDir = "test-suite";
  std::string propertyFile;
  std::string architecture = "64bit";
```

Add to `desc.add_options()` after the `generate-witness` line (line 357):

```cpp
        ("generate-test-suite",
         "\temits a Test-Comp test suite for the violation found")
        ("test-suite-dir", po::value<std::string>()->default_value("test-suite"),
         "\tdirectory to write the test suite into")
        ("property-file", po::value<std::string>(),
         "\tproperty file whose contents go verbatim into <specification>")
        ("architecture", po::value<std::string>()->default_value("64bit"),
         "\tmachine model recorded in the test-suite metadata")
```

And the parsing, next to the other `vm.count` blocks (after line 453):

```cpp
    if (vm.count("generate-test-suite")) {
      args.generateTestSuite = true;
    }
    if (vm.count("test-suite-dir")) {
      args.testSuiteDir = vm["test-suite-dir"].as<std::string>();
    }
    if (vm.count("property-file")) {
      args.propertyFile = vm["property-file"].as<std::string>();
    }
    if (vm.count("architecture")) {
      args.architecture = vm["architecture"].as<std::string>();
    }
```

- [ ] **Step 5: Emit the suite at the violation branch**

Add the includes to `modules/frontend/map2check.cpp`. `<sstream>` is already there
(line 24) but `<fstream>` is not, and `resolveSpecification` needs it:

```cpp
#include <fstream>
```

and, with the other project headers (after line 31):

```cpp
#include "test_suite/test_suite.hpp"
#include "utils/sha256.hpp"
```

Add the two helpers to the **existing** anonymous namespace that opens at line 42 —
do not open a second one:

```cpp
/** The <specification> field. BenchExec always passes a property file, and
 * copying it verbatim is what keeps the metadata correct when the competition
 * revises a property string. The fallbacks exist so that a manual run without
 * --property-file still produces a valid suite. */
std::string resolveSpecification(const std::string& propertyFile,
                                 Map2Check::Map2CheckMode mode) {
  if (!propertyFile.empty()) {
    std::ifstream in(propertyFile);
    if (in.is_open()) {
      std::ostringstream buffer;
      buffer << in.rdbuf();
      std::string text = buffer.str();
      // Property files carry a trailing newline that does not belong in an
      // XML element.
      while (!text.empty() && (text.back() == '\n' || text.back() == '\r')) {
        text.pop_back();
      }
      if (!text.empty()) return text;
    }
    Map2Check::Log::Warning("could not read property file: " + propertyFile);
  }
  if (mode == Map2Check::Map2CheckMode::REACHABILITY_MODE) {
    return "COVER( init(main()), FQL(COVER EDGES(@CALL(reach_error))) )";
  }
  return "COVER( init(main()), FQL(COVER EDGES(@DECISIONEDGE)) )";
}

void emitTestSuite(const std::string& outputDir, const std::string& programFile,
                   const std::string& entryFunction,
                   const std::string& architecture,
                   const std::string& specification, bool coversError) {
  std::vector<std::string> inputs =
      Map2Check::readNonDetLog(Map2Check::kleeLogCSV);

  Map2Check::TestSuiteMetadata metadata;
  metadata.producer = std::string("Map2Check ") + Map2CheckVersion;
  metadata.specification = specification;
  metadata.programFile = programFile;
  metadata.programHash = Map2Check::sha256File(programFile);
  metadata.entryFunction = entryFunction;
  metadata.architecture = architecture;
  metadata.creationTime = Map2Check::isoUtcNow();

  Map2Check::TestSuiteWriter writer(outputDir);
  if (!writer.writeMetadata(metadata)) {
    Map2Check::Log::Warning("could not write test-suite metadata to " + outputDir);
    return;
  }
  if (!writer.writeTestCase(inputs, coversError)) {
    Map2Check::Log::Warning("could not write test case to " + outputDir);
    return;
  }
  Map2Check::Log::Info("Test suite written to " + outputDir + " (" +
                       std::to_string(inputs.size()) + " inputs)");
}
```

In the violation branch (the final `else` around line 308), immediately after
`if (args.generateWitness) generate_witness(...);`:

```cpp
    if (args.generateTestSuite) {
      // Runs before cleanGarbage(): klee_log.csv lives in the scratch directory
      // that cleanGarbage() removes, and the suite must not.
      std::string outputDir = args.testSuiteDir;
      if (!std::filesystem::path(outputDir).is_absolute()) {
        outputDir = caller->getOriginalPath() + "/" + outputDir;
      }
      emitTestSuite(outputDir, caller->c_program_fullpath, args.entryFunction,
                    args.architecture,
                    resolveSpecification(args.propertyFile, args.mode), true);
    }
```

- [ ] **Step 6: Link the new objects into the binary**

In `modules/frontend/CMakeLists.txt`, add to the `add_executable(map2check ...)`
list, after `$<TARGET_OBJECTS:Witness>`:

```cmake
  $<TARGET_OBJECTS:TestSuite>
  $<TARGET_OBJECTS:Sha256>
```

- [ ] **Step 7: Build and run the integration test**

```bash
cd build && ninja && ninja install
cd .. && MAP2CHECK_PATH=$PWD/release bash tests/integration/test_test_suite_emission.sh
```
Expected: PASS on all 12 assertions.

- [ ] **Step 8: Add it to CI**

In `.github/workflows/ci.yml`, in the job that already runs
`tests/integration/test_juliet_sampling.sh`, add the line after it:

```yaml
          bash tests/integration/test_test_suite_emission.sh
```

- [ ] **Step 9: Check style and commit**

```bash
./check_code_style.py -p
git add modules/frontend/caller.hpp modules/frontend/map2check.cpp \
        modules/frontend/CMakeLists.txt \
        tests/integration/test_test_suite_emission.sh .github/workflows/ci.yml
git commit -m "feat(frontend): emit a Test-Comp test suite with --generate-test-suite"
```

---

### Task 4: Prove the suite reproduces the bug, locally

**Files:**
- Create: `tests/testcomp/README.md`
- Create: `tests/testcomp/cover-error-call.prp`
- Create: `tests/testcomp/programs/two_guards.c`
- Create: `tests/testcomp/run_testcov.sh`

**Interfaces:**
- Consumes: `--generate-test-suite` and `--property-file` from Task 3.
- Produces: `tests/testcomp/run_testcov.sh`, taking `<program.c> <property.prp>`,
  exiting 0 when TestCov confirms the suite covers the error. The companion CI-gate
  plan calls this script; nothing else depends on it.

Structural correctness is not reproduction. This task closes the loop with the actual
validator, which is the only authority on whether the suite is worth points. It is
kept local (not CI) because the CI gate, its corpus, and its failure taxonomy are the
companion plan's subject.

- [ ] **Step 1: Write the property file and the program**

`tests/testcomp/cover-error-call.prp`:

```
COVER( init(main()), FQL(COVER EDGES(@CALL(reach_error))) )
```

`tests/testcomp/programs/two_guards.c`:

```c
// Two independent nondeterministic reads guard one error call. A suite that
// gets the values right but the order wrong fails to reach reach_error, so
// this program tests ordering and not only value recovery.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);

int main(void) {
  int a = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();
  if (a == 42) {
    if (b == 7) {
      reach_error();
    }
  }
  return 0;
}
```

- [ ] **Step 2: Write the runner**

`tests/testcomp/run_testcov.sh`:

```bash
#!/bin/bash
# run_testcov.sh <program.c> <property.prp>
#
# Generates a test suite with Map2Check and hands it to TestCov, the official
# Test-Comp validator. Exit 0 only if TestCov confirms the suite covers the
# error -- structural validity is asserted elsewhere; this asks whether the
# suite actually reproduces the bug.
#
# TestCov takes a zip, while the competition consumes a directory, so the zip
# is built here rather than by the tool.
#
# --no-isolation is required in containers without cgroup access (GitHub
# Actions). It disables resource limits, which is acceptable because the outer
# `timeout` bounds the run.

set -u

PROGRAM="$1"
PROPERTY="$2"
MAP2CHECK="${MAP2CHECK_PATH:-/workspace/install_e2e}/bin/map2check"
TESTCOV="${TESTCOV:-testcov}"
BUDGET="${BUDGET:-300}"

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT

cp "$PROGRAM" "$WORK/"
cp "$PROPERTY" "$WORK/"
name=$(basename "$PROGRAM")
prop=$(basename "$PROPERTY")

( cd "$WORK" && timeout -k 10 "$BUDGET" "$MAP2CHECK" \
    --target-function --target-function-name reach_error \
    --generate-test-suite --property-file "$prop" --architecture 64bit \
    --nondet-generator klee --timeout "$((BUDGET / 2))" "$name" ) \
  > "$WORK/map2check.log" 2>&1

if [ ! -d "$WORK/test-suite" ]; then
  echo "FAIL $name: no test suite produced"
  sed -n '$p' "$WORK/map2check.log"
  exit 1
fi

( cd "$WORK/test-suite" && zip -q -r ../suite.zip . )

( cd "$WORK" && "$TESTCOV" --test-suite suite.zip --no-isolation -64 \
    --goal "$prop" "$name" ) > "$WORK/testcov.log" 2>&1
rc=$?

if grep -qi "Cover-Error.*true\|Error covered" "$WORK/testcov.log"; then
  echo "PASS $name: TestCov reproduced the error"
  exit 0
fi

echo "FAIL $name: TestCov did not reproduce the error (testcov rc=$rc)"
tail -20 "$WORK/testcov.log"
exit 1
```

Make it executable: `chmod +x tests/testcomp/run_testcov.sh`.

- [ ] **Step 3: Install TestCov and run it**

```bash
python3 -m venv /tmp/tcvenv && /tmp/tcvenv/bin/pip install testcov
MAP2CHECK_PATH=$PWD/release TESTCOV=/tmp/tcvenv/bin/testcov \
  bash tests/testcomp/run_testcov.sh \
    tests/testcomp/programs/two_guards.c tests/testcomp/cover-error-call.prp
```
Expected: `PASS two_guards.c: TestCov reproduced the error`.

If TestCov reports the suite as valid but not covering, the cause is almost certainly
the input-to-nondet mapping, not the XML. Diagnose by comparing the `<input>` sequence
against `klee_log.csv` from the same run before changing anything in the writer.

- [ ] **Step 4: Write the README**

`tests/testcomp/README.md`:

```markdown
# Test-Comp conformance corpus

Programs and property files used to check that Map2Check emits a test suite the
official validator accepts.

- `cover-error-call.prp` — the Cover-Error property, as the competition states it.
- `programs/` — small programs with a known reachable `reach_error`.
- `run_testcov.sh` — generates a suite with Map2Check, zips it, and asks TestCov
  whether it reproduces the error. Exit 0 means reproduced.

TestCov comes from PyPI (`pip install testcov`, 3.7 at the time of writing) and is
the same tool the competition uses. It needs `zip` and, for coverage measurement,
`gcov`/`lcov`.

Structural validity of the emitted XML is asserted separately, by
`tests/integration/test_test_suite_emission.sh`. This directory answers the other
question: whether the suite is worth points.
```

- [ ] **Step 5: Commit**

```bash
git add tests/testcomp
git commit -m "test(testcomp): validate emitted suites against TestCov locally"
```

---

## Self-review

**Spec coverage.** Every DTD element of `test-metadata` is emitted, in sequence order
(Task 2, `MetadataElementsFollowTheDtdOrder`). Both optional `inputtestsuite*` elements
are deliberately omitted and the reason is recorded in Global Constraints. `testcase`
covers `input*` and `coversError`; `type` and `variable` are deliberately not emitted,
with the `Format.md` "interpretation is undefined" clause as the reason. The directory
layout from `Format.md` ("one `metadata.xml`, one `*.xml` per test case, all in the
same directory") is what `TestSuiteWriter` produces.

**Gaps this plan does not close, by design.**
- *One test case per run.* `map2check_exit()` guards on a static `alreadyReleased`
  (`Map2CheckFunctions.c:82-88`), so the log is flushed once, for the violating
  execution. Cover-Error needs exactly that. **Cover-Branches needs many test cases
  and is out of scope**; it requires per-input logs and is the next plan.
- *No suite on a TRUE or UNKNOWN verdict.* Emission is on the violation branch only.
  Test-Comp expects a suite always, which is gap G4 and belongs with the
  Cover-Branches work.
- *KLEE only, in the tests.* The emitter is generator-agnostic because
  `NonDetGeneratorLibFuzzy.c:47` flushes the same log, but the integration test pins
  KLEE for determinism. A LibFuzzer assertion belongs in the CI-gate plan, where
  flakiness can be handled deliberately.

**Placeholder scan.** No TBDs. Every code step carries the code; every run step carries
the command and the expected result.

**Type consistency.** `sha256File` (Task 1) is called in Task 3 with a `std::string`
and returns `std::string`. `readNonDetLog` returns `std::vector<std::string>`, which is
what `writeTestCase` takes. `TestSuiteMetadata`'s seven fields are set in Task 3 in the
same names Task 2 declares. `getOriginalPath()` returns `std::string`, concatenated
with `+` in Task 3. `Map2CheckVersion` is a macro from the generated
`map2check_version.hpp`, already included by `map2check.cpp:38`.

**One risk worth naming.** The integration test asserts the recovered inputs are
exactly `42` and `7`. That holds if KLEE solves both guards and the log records the
solved values. If KLEE instead reports the violation without those exact concretes, the
assertion fails loudly — which is the correct outcome, because a suite with the wrong
values is worth nothing, and finding that out in Task 3 is far cheaper than finding it
out from TestCov in Task 4.
