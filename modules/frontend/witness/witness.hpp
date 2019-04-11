/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#ifndef MODULES_FRONTEND_WITNESS_WITNESS_HPP_
#define MODULES_FRONTEND_WITNESS_WITNESS_HPP_

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

/* This header describes all used witness arguments from
 *  https://github.com/sosy-lab/sv-witnesses
 */

namespace Map2Check {
class DataElement {
 protected:
  virtual std::string convertToString();

 public:
  DataElement() {}
  operator std::string() { return this->convertToString(); }
};

enum class WitnessTypeValues { CORRECTNESS, VIOLATION };

class WitnessType : public DataElement {
 protected:
  WitnessTypeValues witnessType;
  virtual std::string convertToString();

 public:
  explicit WitnessType(WitnessTypeValues witnessType)
      : DataElement(), witnessType(witnessType) {}
};

enum class SupportedSourceCodeLang { C };

class SourceCodeLang : public DataElement {
 protected:
  SupportedSourceCodeLang language;
  virtual std::string convertToString();

 public:
  explicit SourceCodeLang(SupportedSourceCodeLang language)
      : DataElement(), language(language) {}
};

class Producer : public DataElement {
 protected:
  virtual std::string convertToString();

 public:
  Producer() : DataElement() {}
};

enum class SpecificationType {
  FREE,
  MEMLEAK,
  DEREF,
  TARGET,
  SPECOVERFLOW,
  MEMSAFETY
};

class Specification : public DataElement {
 protected:
  std::string value;
  virtual std::string convertToString();

 public:
  // TODO(hbgit): SHOULD THROW ERROR
  explicit Specification(SpecificationType type) : DataElement() {
    switch (type) {
      case SpecificationType::FREE:
        this->value = "CHECK( init(main()), LTL(G valid-free) )";
        break;
      case SpecificationType::MEMLEAK:
        this->value = "CHECK( init(main()), LTL(G valid-memtrack) )";
        break;
      case SpecificationType::DEREF:
        this->value = "CHECK( init(main()), LTL(G valid-deref) )";
        break;
      case SpecificationType::TARGET:
        this->value = "ERROR";
        break;
      case SpecificationType::SPECOVERFLOW:
        this->value = "CHECK( init(main()), LTL(G ! overflow) )";
        break;
      case SpecificationType::MEMSAFETY:
        this->value =
            "CHECK( init(main()), LTL(G valid-free) )\nCHECK( init(main()), "
            "LTL(G valid-deref) )\nCHECK( init(main()), LTL(G valid-memtrack) "
            ")";
        break;
    }
  }
  // TODO(hbgit): SHOULD THROW ERROR
  Specification(SpecificationType type, std::string target) : DataElement() {
    switch (type) {
      case SpecificationType::FREE:
        this->value = "ERROR";
        break;
      case SpecificationType::MEMLEAK:
        this->value = "ERROR";
        break;
      case SpecificationType::DEREF:
        this->value = "ERROR";
        break;
      case SpecificationType::SPECOVERFLOW:
        this->value = "ERROR";
        break;
      case SpecificationType::TARGET:
        std::ostringstream cnvt;
        cnvt.str("");
        cnvt << "CHECK( init(main()), LTL(G ! call(";
        cnvt << "__VERIFIER_error";
        cnvt << "())) )";
        this->value = cnvt.str();
        break;
    }
  }
};

class ProgramFile : public DataElement {
 protected:
  std::string path;
  virtual std::string convertToString();

 public:
  explicit ProgramFile(std::string path) : path(path), DataElement() {}
};

class ProgramHash : public DataElement {
 protected:
  std::string hash;
  virtual std::string convertToString();

 public:
  explicit ProgramHash(std::string hash) : hash(hash), DataElement() {}
};

enum class ArchitectureType { Bit32, Bit64 };

class Architecture : public DataElement {
 protected:
  ArchitectureType type;
  virtual std::string convertToString();

 public:
  explicit Architecture(ArchitectureType type) : DataElement(), type(type) {}
};

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_WITNESS_WITNESS_HPP_
