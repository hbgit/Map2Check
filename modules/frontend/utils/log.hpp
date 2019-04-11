/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#ifndef MODULES_FRONTEND_UTILS_LOG_HPP_
#define MODULES_FRONTEND_UTILS_LOG_HPP_

#include <string>

namespace Map2Check {
// TODO(rafa.sa.xp@gmail.com): make logging levels
class Log {
 private:
  static bool isDebugMode;

 public:
  static void ActivateDebugMode() { isDebugMode = true; }
  static void Error(std::string msg);
  static void Warning(std::string msg);
  static void Debug(std::string msg);
  static void Info(std::string msg);
  static void Fatal(std::string msg);
};

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_UTILS_LOG_HPP_
