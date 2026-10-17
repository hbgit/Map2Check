/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0 AND Apache-2.0)
 **/

#ifndef MODULES_FRONTEND_UTILS_CLANG_HPP_
#define MODULES_FRONTEND_UTILS_CLANG_HPP_

#include <sstream>
#include <string>

namespace Map2Check {

/// Flags every clang invocation that turns the analysed program's C into
/// bitcode must pass (map2check-library D1, condition 11). The runtime
/// rejects a fuzzer input whose __VERIFIER_assume fails by unwinding through
/// the program's frames; without -fexceptions clang marks every C function
/// nounwind, and unwinding through a nounwind function is undefined in LLVM
/// IR. The attributes travel in the bitcode, so the flag belongs on the
/// -emit-llvm step. Never pair it with -fno-unwind-tables or
/// -fno-asynchronous-unwind-tables.
inline constexpr const char* kProgramUnwindFlags = "-fexceptions";

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
