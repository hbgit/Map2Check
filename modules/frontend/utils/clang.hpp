#pragma once

#include <string>
#include <sstream>

namespace Map2Check {

inline
std::string GenerateIRCode(
    std::string clang_path,
    std::string include_dir,
    std::string program) {
  std::ostringstream command;
  command.str("");

  command << clang_path
          << "clang -c -emit-llvm "
          << "-I " << include_dir << " "
          << program;

  return command.str();
}

inline
std::string LinkIR(
  std::string opt_path,
  std::string ir_program);

}  // namespace Map2Check
