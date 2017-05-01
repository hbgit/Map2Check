#pragma once

#include <iostream>
using namespace std;

namespace Map2Check::Exceptions {
  class Map2CheckException : public runtime_error {
    public:
      Map2CheckException(string message) : runtime_error(message) {}
      virtual const char* what() const throw();
  };

  class InvalidKleeFolderException : public Map2CheckException {
    public:
      InvalidKleeFolderException(string path) : Map2CheckException("Could not open: " + path), path(path) {}
      // virtual const char* what() const throw();
    private:
      string path;
  };

  

  class ErrorOpeningFileException : public Map2CheckException {
    public:
      ErrorOpeningFileException(string path) : Map2CheckException("Could not open: " + path), path(path) {}
      // virtual const char* what() const throw();
    private:
      string path;
  };

  class OutOfBounds : public Map2CheckException {
    public:
      OutOfBounds() : Map2CheckException("Index out of bounds") {}
  };
}
