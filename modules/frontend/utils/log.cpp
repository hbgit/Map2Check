/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include "log.hpp"
#include <iostream>
#include <string>

namespace Map2Check {

void Log::Error(std::string msg) {
  std::cout << "\033[0;31m" << msg << "\033[0m\n";
}

void Log::Warning(std::string msg) {
  std::cout << "\033[1;33m" << msg << "\033[0m\n";
}

void Log::Debug(std::string msg) {
  if (isDebugMode) std::cout << "\033[0;33m" << msg << "\033[0m\n";
}

void Log::Info(std::string msg) {
  std::cout << "\033[0;32m" << msg << "\033[0m\n";
}

void Log::Fatal(std::string msg) {
  std::cout << "\033[0;31m" << msg << "\033[0m\n";
}

bool Log::isDebugMode = false;
}  // namespace Map2Check
