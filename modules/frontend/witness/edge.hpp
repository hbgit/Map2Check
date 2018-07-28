#pragma once
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
  StartLine(std::string line) : line(line), EdgeData() {}
};

class SourceCode : public EdgeData {
 protected:
  std::string sourcecode;
  virtual std::string convertToString();

 public:
  SourceCode(std::string sourcecode) : sourcecode(sourcecode), EdgeData() {}
};

class Control : public EdgeData {
 protected:
  std::string control;
  virtual std::string convertToString();

 public:
  Control(std::string control) : control(control), EdgeData() {}
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
