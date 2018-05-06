#include "afl.hpp"

#include <sstream>

void Map2Check::AFL_COMPILE::setPath(std::string path) {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << path;
  cnvt << "afl-clang";
  this->afl_clang = cnvt.str();
}

std::string Map2Check::AFL_COMPILE::getCommand() {
  return this->afl_clang;
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

/*
std::string Map2Check::AFL_EXEC::getCommand() {
  std::ostringstream cnvt;
  cnvt.str("");
  if()
}
*/
