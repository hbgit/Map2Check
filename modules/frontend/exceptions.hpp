/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#ifndef MODULES_FRONTEND_EXCEPTIONS_HPP_
#define MODULES_FRONTEND_EXCEPTIONS_HPP_

#include <iostream>
#include <string>

using std::string;

namespace Map2Check::Exceptions {
class Map2CheckException : public runtime_error {
 public:
  explicit Map2CheckException(string message) : runtime_error(message) {}
  virtual const char* what() const throw();
};

class ErrorOpeningFileException : public Map2CheckException {
 public:
  explicit ErrorOpeningFileException(string path)
      : Map2CheckException("Could not open: " + path), path(path) {}
  // virtual const char* what() const throw();
 private:
  string path;
};

class OutOfBounds : public Map2CheckException {
 public:
  OutOfBounds() : Map2CheckException("Index out of bounds") {}
};
}  // namespace Map2Check::Exceptions

#endif  // MODULES_FRONTEND_EXCEPTIONS_HPP_
