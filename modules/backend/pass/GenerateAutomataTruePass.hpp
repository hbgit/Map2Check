/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#ifndef MODULES_BACKEND_PASS_GENERATEAUTOMATATRUEPASS_HPP_
#define MODULES_BACKEND_PASS_GENERATEAUTOMATATRUEPASS_HPP_

#include <llvm/IR/Constants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/Module.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>

#include <memory>
#include <string>
#include <vector>

#include <boost/algorithm/string/replace.hpp>

// From Map2Check Project
#include "../../frontend/utils/tools.hpp"
#include "DebugInfo.hpp"

// using namespace llvm;
namespace Tools = Map2Check;

using llvm::BasicBlock;
using llvm::Function;
using llvm::FunctionPass;
using llvm::ICmpInst;
using llvm::Instruction;
using llvm::LLVMContext;
using llvm::make_unique;

struct GenerateAutomataTruePass : public FunctionPass {
  static char ID;
  GenerateAutomataTruePass() : FunctionPass(ID) {}
  explicit GenerateAutomataTruePass(std::string c_program_path)
      : FunctionPass(ID) {
    this->c_program_path = c_program_path;
    this->sourceCodeHelper = make_unique<Tools::SourceCodeHelper>(
        Tools::SourceCodeHelper(c_program_path));
  }

  virtual bool runOnFunction(Function& F);

 protected:
  void runOnBasicBlock(BasicBlock& B, LLVMContext* Ctx);
  void hasCallOnBasicBlock(BasicBlock& B, LLVMContext* Ctx);
  bool checkBBHasLError(BasicBlock& nowB);
  void checkAndSkipAssume();
  void skipEmptyLine();
  BasicBlock::iterator& skipEmptyLineIt(BasicBlock::iterator& iT);
  bool checkInstBbIsAssume(BasicBlock::iterator& iT);
  bool isBranchCond(BasicBlock& B);
  std::string convertLLPredicateToXmlText(Instruction& I);
  std::string getPredicateSymOnXmlText(ICmpInst& icmpInst);
  void identifyAssertLoc(BasicBlock& B);
  void printStateData();
  void resetStateData();

 private:
  // attr to automata state
  std::string replaceCodeByXml(std::string sourceCodeTxt);
  BasicBlock::iterator st_lastBlockInst;
  std::string st_currentFunctionName;
  int st_numLineEntryPoint;
  int st_startline;
  int st_numLineBeginBB;
  int st_numLine2NextSt = 0;
  std::string st_sourceCodeLine;
  std::string st_controlCode;
  int st_numLineGoControl_1true;
  int st_numLineGoControl_2false;
  bool st_isControl = false;
  bool st_isEntryPoint = false;
  int st_isErrorLocation = 0;

  // others
  int numLineBlk_ori;
  int numLineBlk_AA;
  int numLineControlSt;
  bool enableDataBlk = false;
  bool isPredicateNe = false;
  int countEntryPoint = 1;
  Function* currentFunction;
  std::unique_ptr<Tools::SourceCodeHelper> sourceCodeHelper;
  std::vector<int> assertListLoc;
  std::string c_program_path;
  BasicBlock::iterator currentInstruction;
  BasicBlock::iterator lastInstructionMain;
  BasicBlock::iterator firstBlockInst;
  int actualSizeBB;
  int skipEmptyLineItSize;
  LLVMContext* Ctx;
};

#endif  // MODULES_BACKEND_PASS_GENERATEAUTOMATATRUEPASS_HPP_
