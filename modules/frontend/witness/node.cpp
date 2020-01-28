/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include <memory>
#include <string>
#include <utility>

#include "node.hpp"

// using namespace Map2Check;
using Map2Check::EntryNode;
using Map2Check::Node;
using Map2Check::NodeElement;
using Map2Check::SinkNode;
using Map2Check::ViolationNode;
using Map2Check::ViolationNodeElement;

std::string NodeElement::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "NodeElement";
  return cnvt.str();
}

std::string ViolationNodeElement::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "ViolationNodeElement";
  return cnvt.str();
}

std::string EntryNode::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "\t\t\t<data key=\"entry\">true</data>";
  return cnvt.str();
}

std::string SinkNode::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "\t\t\t<data key=\"sink\">true</data>";
  return cnvt.str();
}

std::string ViolationNode::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "\t\t\t<data key=\"violation\">true</data>";
  return cnvt.str();
}

std::string Node::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "\t\t<node id=\"";
  cnvt << this->id;
  cnvt << "\"";

  if (this->elements.size() == 0) {
    cnvt << "/>";
  } else {
    cnvt << ">\n";
    for (int i = 0; i < this->elements.size(); i++) {
      cnvt << (std::string) * this->elements[i];
      cnvt << "\n";
    }
    cnvt << "\t\t</node>";
  }
  return cnvt.str();
}

void Node::AddElement(std::unique_ptr<NodeElement> element) {
  this->elements.push_back(std::move(element));
}
