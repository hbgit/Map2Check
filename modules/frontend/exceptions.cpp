/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#include <sstream>
#include "exceptions.hpp"
#include "utils/log.hpp"
using namespace Map2Check::Exceptions;

const char* Map2CheckException::what() const throw() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << runtime_error::what();
  Map2Check::Log::Error(cnvt.str());
  return cnvt.str().c_str();
}
