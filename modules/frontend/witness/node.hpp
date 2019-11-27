/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#ifndef MODULES_FRONTEND_WITNESS_NODE_HPP_
#define MODULES_FRONTEND_WITNESS_NODE_HPP_

#include <memory>
#include <string>
#include <vector>
#include "witness.hpp"
namespace Map2Check {
class NodeElement {
 protected:
  virtual std::string convertToString();

 public:
  NodeElement() {}
  operator std::string() { return this->convertToString(); }
};

class ViolationNodeElement : public NodeElement {
 protected:
  virtual std::string convertToString();

 public:
  ViolationNodeElement() : NodeElement() {}
};

class EntryNode : public ViolationNodeElement {
 protected:
  virtual std::string convertToString();

 public:
  EntryNode() : ViolationNodeElement() {}
};

class SinkNode : public ViolationNodeElement {
 protected:
  virtual std::string convertToString();

 public:
  SinkNode() : ViolationNodeElement() {}
};

class ViolationNode : public ViolationNodeElement {
 protected:
  virtual std::string convertToString();

 public:
  ViolationNode() : ViolationNodeElement() {}
};

class Node {
 protected:
  std::string id;
  std::vector<std::unique_ptr<NodeElement>> elements;
  virtual std::string convertToString();

 public:
  explicit Node(std::string id) : id(id) {}
  void AddElement(std::unique_ptr<NodeElement> element);
  operator std::string() { return this->convertToString(); }
};

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_WITNESS_NODE_HPP_
