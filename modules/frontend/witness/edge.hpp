/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#ifndef MODULES_FRONTEND_WITNESS_EDGE_HPP_
#define MODULES_FRONTEND_WITNESS_EDGE_HPP_

#include <memory>
#include <string>
#include <vector>
#include "node.hpp"

namespace Map2Check {
class EdgeData {
 protected:
  virtual std::string convertToString();

 public:
  EdgeData() {}
  operator std::string() { return this->convertToString(); }
};

class StartLine : public EdgeData {
 protected:
  std::string line;
  virtual std::string convertToString();

 public:
  explicit StartLine(std::string line) : line(line), EdgeData() {}
};

class SourceCode : public EdgeData {
 protected:
  std::string sourcecode;
  virtual std::string convertToString();

 public:
  explicit SourceCode(std::string sourcecode)
      : sourcecode(sourcecode), EdgeData() {}
};

class Control : public EdgeData {
 protected:
  std::string control;
  virtual std::string convertToString();

 public:
  explicit Control(std::string control) : control(control), EdgeData() {}
};

class AssumptionEdgeData : public EdgeData {
 protected:
  std::string result;
  std::string functionName;
  std::string scope;
  virtual std::string convertToString();

 public:
  AssumptionEdgeData(std::string result, std::string functionName,
                     std::string scope)
      : result(result), functionName(functionName), scope(scope), EdgeData() {}
};

class Edge {
 protected:
  std::string source;
  std::string target;
  std::vector<std::unique_ptr<EdgeData>> elements;
  virtual std::string convertToString();

 public:
  Edge(std::string source, std::string target)
      : source(source), target(target) {}
  void AddElement(std::unique_ptr<EdgeData> element);
  operator std::string() { return this->convertToString(); }
};

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_WITNESS_EDGE_HPP_
