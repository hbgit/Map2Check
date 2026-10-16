/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

// Reader for docs/cutover/abi-mapping.tsv of map2check-library: columns
// c_symbol, core_symbol (several joined by '+', '-' for none), owner_task,
// notes. Lines starting with '#' are comments.

#ifndef TESTS_UNIT_PASS_ABIMAPPING_HPP_
#define TESTS_UNIT_PASS_ABIMAPPING_HPP_

#include <fstream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace m2c_test {

struct AbiRow {
  std::string cSymbol;
  std::vector<std::string> coreSymbols;  // empty when the core is '-'
};

inline std::vector<AbiRow> loadAbiMapping(const std::string& path) {
  std::ifstream in(path);
  if (!in.is_open()) throw std::runtime_error("cannot open " + path);
  std::vector<AbiRow> rows;
  std::string line;
  bool header = true;
  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#') continue;
    if (header) {  // c_symbol  core_symbol  owner_task  notes
      header = false;
      continue;
    }
    std::istringstream fields(line);
    AbiRow row;
    std::string core;
    std::getline(fields, row.cSymbol, '\t');
    std::getline(fields, core, '\t');
    if (core != "-") {
      std::istringstream parts(core);
      std::string part;
      while (std::getline(parts, part, '+')) {
        if (!part.empty()) row.coreSymbols.push_back(part);
      }
    }
    rows.push_back(std::move(row));
  }
  return rows;
}

/// Every name in the core_symbol column.
inline std::set<std::string> loadCoreSymbols(const std::string& path) {
  std::set<std::string> core;
  for (const AbiRow& row : loadAbiMapping(path)) {
    core.insert(row.coreSymbols.begin(), row.coreSymbols.end());
  }
  return core;
}

/// Every c_symbol that is not also a core symbol: the names of the C runtime
/// that the passes must no longer emit.
inline std::set<std::string> loadLegacyOnlySymbols(const std::string& path) {
  std::set<std::string> core = loadCoreSymbols(path);
  std::set<std::string> legacy;
  for (const AbiRow& row : loadAbiMapping(path)) {
    if (core.count(row.cSymbol) == 0) legacy.insert(row.cSymbol);
  }
  return legacy;
}

}  // namespace m2c_test

#endif  // TESTS_UNIT_PASS_ABIMAPPING_HPP_
