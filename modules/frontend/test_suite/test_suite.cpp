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
    : directory(std::move(directory)), counter(0) {}

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
                                    bool coversError) {
  std::error_code ec;
  std::filesystem::create_directories(this->directory, ec);

  this->counter++;
  std::string name = "testcase-" + std::to_string(this->counter) + ".xml";
  std::ofstream out(std::filesystem::path(this->directory) / name);
  if (!out.is_open()) return false;

  out << kXmlDeclaration << "\n" << kTestCaseDoctype << "\n";
  // coversError defaults to "false" in the DTD, so it is only spelled out when
  // the run actually reached the error.
  out << (coversError ? "<testcase coversError=\"true\">\n" : "<testcase>\n");
  // Order is the entire contract: the n-th <input> is the value returned by
  // the n-th nondet call.
  for (const std::string& value : inputs) {
    out << "  <input>" << escapeXml(value) << "</input>\n";
  }
  out << "</testcase>\n";
  out.close();
  return out.good();
}

}  // namespace Map2Check
