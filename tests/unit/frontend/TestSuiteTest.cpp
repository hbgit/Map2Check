/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

// A malformed test suite does not fail loudly -- the validator simply scores
// it as covering nothing, which is indistinguishable from the tool having
// found no bug. These assertions pin the parts of the format whose violation
// would be invisible at run time: element order (the DTD is a sequence),
// the public identifiers, and above all the ORDER of <input> elements, which
// is the whole semantics of a test vector.

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

// Each test owns its directory so nothing leaks between cases.
fs::path freshDir(const std::string& name) {
  fs::path d = fs::temp_directory_path() / ("m2c_ts_" + name);
  fs::remove_all(d);
  return d;
}

Map2Check::TestSuiteMetadata sampleMetadata() {
  Map2Check::TestSuiteMetadata md;
  md.producer = "Map2Check v8.0.0";
  md.specification =
      "COVER( init(main()), FQL(COVER EDGES(@CALL(reach_error))) )";
  md.programFile = "./digits.c";
  md.programHash =
      "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
  md.entryFunction = "main";
  md.architecture = "64bit";
  md.creationTime = "2026-08-16T12:00:00Z";
  return md;
}

}  // namespace

// --- metadata.xml -----------------------------------------------------------

TEST(TestSuiteWriter, MetadataStartsWithDeclarationThenDoctype) {
  fs::path d = freshDir("meta_head");
  Map2Check::TestSuiteWriter w(d.string());
  ASSERT_TRUE(w.writeMetadata(sampleMetadata()));

  std::string xml = slurp(d / "metadata.xml");
  EXPECT_EQ(
      xml.find("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"),
      0u);
  EXPECT_NE(
      xml.find("<!DOCTYPE test-metadata PUBLIC "
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
  const std::vector<std::string> order = {"<sourcecodelang>", "<producer>",
                                          "<specification>",  "<programfile>",
                                          "<programhash>",    "<entryfunction>",
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
  EXPECT_NE(xml.find("<producer>Map2Check v8.0.0</producer>"),
            std::string::npos);
  EXPECT_NE(xml.find("<programfile>./digits.c</programfile>"),
            std::string::npos);
  EXPECT_NE(xml.find("<architecture>64bit</architecture>"), std::string::npos);
  EXPECT_NE(xml.find("<creationtime>2026-08-16T12:00:00Z</creationtime>"),
            std::string::npos);
  fs::remove_all(d);
}

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
  EXPECT_EQ(
      xml.find("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"),
      0u);
  EXPECT_NE(
      xml.find("<!DOCTYPE testcase PUBLIC "
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

// A program with no nondeterministic input still needs a test case. Dropping
// it would leave a suite with nothing to execute, which scores zero even
// though the error is trivially reachable.
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

TEST(TestSuiteWriter, CreatesNestedDirectoriesIfAbsent) {
  fs::path root = freshDir("tc_mkdir");
  fs::path d = root / "nested" / "test-suite";
  Map2Check::TestSuiteWriter w(d.string());
  ASSERT_TRUE(w.writeTestCase({"1"}, false));
  EXPECT_TRUE(fs::exists(d / "testcase-1.xml"));
  fs::remove_all(root);
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

// A log truncated by a crash or a budget kill still yields a usable prefix.
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

TEST(IsoUtcNow, MatchesTheFormatTheFormatExpects) {
  std::string now = Map2Check::isoUtcNow();
  ASSERT_EQ(now.size(), 20u);  // 2026-08-16T12:00:00Z
  EXPECT_EQ(now[4], '-');
  EXPECT_EQ(now[7], '-');
  EXPECT_EQ(now[10], 'T');
  EXPECT_EQ(now[13], ':');
  EXPECT_EQ(now[16], ':');
  EXPECT_EQ(now[19], 'Z');
}
