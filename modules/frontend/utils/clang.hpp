/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#ifndef MODULES_FRONTEND_UTILS_CLANG_HPP_
#define MODULES_FRONTEND_UTILS_CLANG_HPP_

#include <sstream>
#include <string>

namespace Map2Check {

inline std::string GenerateIRCode(std::string clang_path,
                                  std::string include_dir,
                                  std::string program) {
  std::ostringstream command;
  command.str("");

  command << clang_path << "clang -c -emit-llvm "
          << "-I " << include_dir << " " << program;

  return command.str();
}

inline std::string LinkIR(std::string opt_path, std::string ir_program);

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_UTILS_CLANG_HPP_
