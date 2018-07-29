#include "log.hpp"
#include <iostream>
#include <string>

namespace Map2Check {

void Log::Error(std::string msg) {
  std::cout << "\033[0;31m" << msg << "\033[0m";
}

void Log::Warning(std::string msg) {
  std::cout << "\033[1;33m" << msg << "\033[0m";
}

void Log::Debug(std::string msg) {
  std::cout << "\033[0;33m" << msg << "\033[0m";
}

void Log::Info(std::string msg) {
  std::cout << "\033[0;32m" << msg << "\033[0m";
}

void Log::Fatal(std::string msg) {
  std::cout << "\033[0;31m" << msg << "\033[0m";
}

}  // namespace Map2Check
