/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#include <memory>
#include <string>
#include <utility>

#include "edge.hpp"
#include "witness.hpp"

using namespace Map2Check;
void Edge::AddElement(std::unique_ptr<EdgeData> element) {
  this->elements.push_back(std::move(element));
}

std::string EdgeData::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "EdgeData";
  return cnvt.str();
}

std::string StartLine::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "\t\t\t<data key=\"startline\">";
  cnvt << this->line;
  cnvt << "</data>";

  return cnvt.str();
}

std::string SourceCode::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "\t\t\t<data key=\"sourcecode\">";
  cnvt << this->sourcecode;
  cnvt << "</data>";

  return cnvt.str();
}

std::string Control::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "\t\t\t<data key=\"control\">";
  cnvt << this->control;
  cnvt << "</data>";

  return cnvt.str();
}

std::string AssumptionEdgeData::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "\t\t\t<data key=\"assumption\">\\result == ";
  cnvt << this->result;
  cnvt << "</data>\n";

  cnvt << "\t\t\t<data key=\"assumption.scope\">";
  cnvt << this->scope;
  cnvt << "</data>\n";

  cnvt << "\t\t\t<data key=\"assumption.resultfunction\">";
  cnvt << this->functionName;
  cnvt << "</data>";

  return cnvt.str();
}

std::string Edge::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "\t\t<edge source=\"";
  cnvt << this->source;
  cnvt << "\" target=\"";
  cnvt << this->target;
  cnvt << "\"";

  if (this->elements.size() == 0) {
    cnvt << "/>";
  } else {
    cnvt << ">\n";
    for (int i = 0; i < this->elements.size(); i++) {
      cnvt << (std::string) * this->elements[i];
      cnvt << "\n";
    }
    cnvt << "\t\t</edge>";
  }
  return cnvt.str();
}
