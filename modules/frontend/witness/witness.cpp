/**
* Copyright (C) 2014 - 2019 Map2Check tool
* This file is part of the Map2Check tool, and is made available under
* the terms of the GNU General Public License version 3.
**/
#include "witness.hpp"
#include <algorithm>
#include <fstream>
#include <memory>
#include <string>
#include "../utils/log.hpp"
#include "../utils/tools.hpp"

using namespace Map2Check;

std::string DataElement::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "DataElement";
  return cnvt.str();
}

std::string WitnessType::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "\t\t<data key=\"witness-type\">";

  switch (this->witnessType) {
    case WitnessTypeValues::CORRECTNESS:
      cnvt << "correctness_witness";
      break;
    case WitnessTypeValues::VIOLATION:
      cnvt << "violation_witness";
      break;
  }
  cnvt << "</data>";
  return cnvt.str();
}

std::string SourceCodeLang::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "\t\t<data key=\"sourcecodelang\">";

  switch (this->language) {
    case SupportedSourceCodeLang::C:
      cnvt << "C";
      break;
  }
  cnvt << "</data>";
  return cnvt.str();
}

std::string Producer::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "\t\t<data key=\"producer\">";
  cnvt << "Map2Check";
  cnvt << "</data>";
  return cnvt.str();
}

std::string Specification::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "\t\t<data key=\"specification\">";
  cnvt << this->value;
  cnvt << "</data>";
  return cnvt.str();
}

std::string ProgramFile::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "\t\t<data key=\"programfile\">";
  cnvt << this->path;
  cnvt << "</data>";
  return cnvt.str();
}

std::string ProgramHash::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "\t\t<data key=\"programhash\">";
  cnvt << this->hash;
  cnvt << "</data>";
  return cnvt.str();
}

std::string Architecture::convertToString() {
  std::ostringstream cnvt;
  cnvt.str("");
  cnvt << "\t\t<data key=\"architecture\">";
  switch (this->type) {
    case ArchitectureType::Bit32:
      cnvt << "32bit";
      break;
    case ArchitectureType::Bit64:
      cnvt << "64bit";
      break;
  }
  cnvt << "</data>";
  return cnvt.str();
}
