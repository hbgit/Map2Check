/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#include "afl.hpp"

#include <sstream>
#include <string>

void Map2Check::AFL_COMPILE::setPath(std::string path) {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << path;
  cnvt << "afl-clang";
  this->afl_clang = cnvt.str();
}

std::string Map2Check::AFL_COMPILE::getCommand() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << this->afl_clang;
  if (compileTo32Bits) {
    cnvt << " -m32";
  }

  return cnvt.str();
}

void Map2Check::AFL_EXEC::setPath(std::string path) {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << path;
  cnvt << "afl-fuzz";
  this->afl_fuzz = cnvt.str();
}

void Map2Check::AFL_EXEC::setTimeout(u_int32_t timeout) {
  this->timeout = timeout;
}

void Map2Check::AFL_EXEC::setBenchUntilCrash(bool isOn) {
  this->benchUntilCrash = isOn;
}

void Map2Check::AFL_EXEC::setSkipCpuFreq(bool isOn) {
  this->skipCpuFreq = isOn;
}

std::string Map2Check::AFL_EXEC::getCommand() {
  std::ostringstream cnvt;
  cnvt.str("");

  if (benchUntilCrash) {
    cnvt << "AFL_BENCH_UNTIL_CRASH=1 ";
  }

  if (skipCpuFreq) {
    cnvt << "AFL_SKIP_CPUFREQ=1 ";
  }

  if (timeout != 0) {
    cnvt << "timeout " << timeout << " ";
  }

  cnvt << afl_fuzz;
  return cnvt.str();
}
