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

/* Every message is flushed, and that is the whole point of this file.
 *
 * stdout is block-buffered whenever it is not a terminal, and every harness
 * here redirects it to a file. A run killed by the outer `timeout` -- which is
 * the normal end of a run that overruns its budget -- therefore lost its
 * ENTIRE log, verdict included, because nothing had reached the file yet.
 *
 * Measured on the v12 Test-Comp corpus: 20 tasks recorded ERROR, every one of
 * them at 87 to 89 seconds against a 90 second outer timeout, and their logs
 * were zero bytes. The harness reads a missing VERIFICATION line as a crashed
 * tool. Some of those runs had already decided.
 *
 * These messages are a few dozen per run, so flushing each one costs nothing
 * worth measuring. */

namespace Map2Check {

void Log::Error(std::string msg) {
  std::cout << "\033[0;31m" << msg << "\033[0m\n" << std::flush;
}

void Log::Warning(std::string msg) {
  std::cout << "\033[1;33m" << msg << "\033[0m\n" << std::flush;
}

void Log::Debug(std::string msg) {
  if (isDebugMode) std::cout << "\033[0;33m" << msg << "\033[0m\n" << std::flush;
}

void Log::Info(std::string msg) {
  std::cout << "\033[0;32m" << msg << "\033[0m\n" << std::flush;
}

void Log::Fatal(std::string msg) {
  std::cout << "\033[0;31m" << msg << "\033[0m\n" << std::flush;
}

bool Log::isDebugMode = false;
}  // namespace Map2Check
