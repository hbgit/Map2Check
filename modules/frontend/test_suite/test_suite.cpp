/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include "test_suite.hpp"

#include <algorithm>
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

/** Marks a case as an assumption-pruned path (see evictPrunedCase). A comment
 * is outside the test format, so validators and TestCov ignore it. */
const char kPrunedComment[] = "<!-- map2check: assumption-pruned path -->";

/** Field index of the value in a nondet log row.
 * NonDetLog.c writes: id;line;scope;function_name;step;value;type */
constexpr size_t kValueField = 5;
constexpr size_t kExpectedFields = 7;

/** Nothing here is attacker-controlled, but a stray '&' in a program path
 * would still produce a document no validator accepts -- and the failure would
 * surface as a rejected suite rather than as an error from this code. */
std::string escapeXml(const std::string& raw) {
  std::string out;
  out.reserve(raw.size());
  for (char c : raw) {
    switch (c) {
      case '&':
        out += "&amp;";
        break;
      case '<':
        out += "&lt;";
        break;
      case '>':
        out += "&gt;";
        break;
      case '"':
        out += "&quot;";
        break;
      case '\'':
        out += "&apos;";
        break;
      default:
        out += c;
    }
  }
  return out;
}

}  // namespace

std::string isoUtcNow() {
  std::time_t now = std::time(nullptr);
  std::tm utc{};
  gmtime_r(&now, &utc);
  char buffer[21];
  std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &utc);
  return std::string(buffer);
}

std::vector<std::string> readNonDetLog(const std::string& csvPath) {
  std::vector<std::string> values;
  std::ifstream in(csvPath);
  if (!in.is_open()) return values;

  std::string line;
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    std::vector<std::string> fields;
    std::string field;
    std::istringstream row(line);
    while (std::getline(row, field, ';')) fields.push_back(field);
    if (fields.size() < kExpectedFields) continue;
    values.push_back(fields[kValueField]);
  }
  return values;
}

TestSuiteWriter::TestSuiteWriter(std::string directory)
    : directory(std::move(directory)), counter(0) {
  // After the cases already there: with the engines alternating, more than one
  // phase writes into the same suite, and counting from 1 again overwrote the
  // earlier phase's cases (tacas 3b spec).
  std::error_code ec;
  for (const auto& entry :
       std::filesystem::directory_iterator(this->directory, ec)) {
    const std::string name = entry.path().filename().string();
    static const std::string kPrefix = "testcase-";
    static const std::string kSuffix = ".xml";
    if (name.size() <= kPrefix.size() + kSuffix.size() ||
        name.compare(0, kPrefix.size(), kPrefix) != 0 ||
        name.compare(name.size() - kSuffix.size(), kSuffix.size(), kSuffix) !=
            0) {
      continue;
    }
    const std::string digits = name.substr(
        kPrefix.size(), name.size() - kPrefix.size() - kSuffix.size());
    if (digits.empty() || digits.size() > 9 ||
        digits.find_first_not_of("0123456789") != std::string::npos) {
      continue;
    }
    this->counter = std::max(
        this->counter, static_cast<unsigned>(std::stoul(digits)));
    // Its inputs, as written (escaped), so a later phase can skip a vector
    // the suite already has.
    std::ifstream in(entry.path());
    std::stringstream text;
    text << in.rdbuf();
    const std::string xml = text.str();
    std::vector<std::string> inputs;
    static const std::string kOpen = "<input>", kClose = "</input>";
    for (size_t at = xml.find(kOpen); at != std::string::npos;
         at = xml.find(kOpen, at)) {
      const size_t end = xml.find(kClose, at + kOpen.size());
      if (end == std::string::npos) break;
      inputs.push_back(xml.substr(at + kOpen.size(), end - at - kOpen.size()));
      at = end + kClose.size();
    }
    this->cases.insert(inputs);
    if (xml.find(kPrunedComment) != std::string::npos) {
      this->prunedCases[inputs] = entry.path();
    }
  }
}

bool TestSuiteWriter::evictPrunedCase() {
  if (this->prunedCases.empty()) return false;
  auto victim = this->prunedCases.begin();
  std::error_code ec;
  std::filesystem::remove(victim->second, ec);
  this->cases.erase(victim->first);
  this->prunedCases.erase(victim);
  return true;
}

namespace {
std::vector<std::string> escapedInputs(const std::vector<std::string>& raw) {
  std::vector<std::string> escaped;
  escaped.reserve(raw.size());
  for (const std::string& value : raw) escaped.push_back(escapeXml(value));
  return escaped;
}
}  // namespace

bool TestSuiteWriter::hasTestCase(
    const std::vector<std::string>& inputs) const {
  return this->cases.count(escapedInputs(inputs)) > 0;
}

void TestSuiteWriter::removeTestCases(const std::string& directory) {
  std::error_code ec;
  std::vector<std::filesystem::path> doomed;
  for (const auto& entry : std::filesystem::directory_iterator(directory, ec)) {
    const std::string name = entry.path().filename().string();
    if (name.rfind("testcase-", 0) == 0 && name.size() > 13 &&
        name.compare(name.size() - 4, 4, ".xml") == 0) {
      doomed.push_back(entry.path());
    }
  }
  for (const auto& path : doomed) std::filesystem::remove(path, ec);
}

bool TestSuiteWriter::writeMetadata(const TestSuiteMetadata& metadata) {
  std::error_code ec;
  std::filesystem::create_directories(this->directory, ec);

  std::ofstream out(std::filesystem::path(this->directory) / "metadata.xml");
  if (!out.is_open()) return false;

  // Element order is mandatory: the DTD declares test-metadata as a sequence.
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
  out.close();
  return out.good();
}

bool TestSuiteWriter::writeTestCase(const std::vector<std::string>& inputs,
                                    bool coversError, bool pruned) {
  std::error_code ec;
  std::filesystem::create_directories(this->directory, ec);

  this->counter++;
  std::string name = "testcase-" + std::to_string(this->counter) + ".xml";
  std::ofstream out(std::filesystem::path(this->directory) / name);
  if (!out.is_open()) return false;

  out << kXmlDeclaration << "\n" << kTestCaseDoctype << "\n";
  if (pruned) out << kPrunedComment << "\n";
  // coversError defaults to "false" in the DTD, so it is only spelled out when
  // the run actually reached the error.
  out << (coversError ? "<testcase coversError=\"true\">\n" : "<testcase>\n");
  // Order is the entire contract: the n-th <input> is the value returned by
  // the n-th nondet call.
  for (const std::string& value : inputs) {
    out << "  <input>" << escapeXml(value) << "</input>\n";
  }
  out << "</testcase>\n";
  this->cases.insert(escapedInputs(inputs));
  if (pruned) {
    this->prunedCases[escapedInputs(inputs)] =
        std::filesystem::path(this->directory) / name;
  }
  out.close();
  return out.good();
}

}  // namespace Map2Check
