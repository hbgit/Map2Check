#include "exceptions.hpp"
#include <sstream>
#include "utils/log.hpp"
using namespace Map2Check::Exceptions;

const char* Map2CheckException::what() const throw() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << runtime_error::what();
  Map2Check::Log::Error(cnvt.str());
  return cnvt.str().c_str();
}
