/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#include "GenerateAutomataTruePass.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using llvm::CallInst;
using llvm::cast;
using llvm::dyn_cast;
using llvm::isa;
using llvm::LoadInst;
using llvm::RegisterPass;
using llvm::TerminatorInst;

using std::ofstream;

namespace {
inline Instruction* BBIteratorToInst(BasicBlock::iterator i) {
  Instruction* pointer = reinterpret_cast<Instruction*>(&*i);
  return pointer;
}
}  // namespace

// TODO(hbgit): Skip assume generated by our tool. Keep assumes come from the
// analyzed source code?

bool GenerateAutomataTruePass::runOnFunction(Function& F) {
  this->Ctx = &F.getContext();
  this->currentFunction = &F;
  this->st_currentFunctionName = this->currentFunction->getName();

  int countBB = 1;
  // for (Function::iterator B = F.begin(), e = F.end(); B != e; ++B)
  for (auto& B : F) {
    this->hasCallOnBasicBlock(B, this->Ctx);
    this->runOnBasicBlock(B, this->Ctx);
    this->printStateData();
    countBB++;
  }

  return false;
}

// Replace text code not allowed in XML
std::string GenerateAutomataTruePass::replaceCodeByXml(
    std::string sourceCodeTxt) {
  boost::replace_all(sourceCodeTxt, "&", "&amp;");
  boost::replace_all(sourceCodeTxt, "<", "&lt;");
  boost::replace_all(sourceCodeTxt, ">", "&gt;");
  boost::replace_all(sourceCodeTxt, "<=", "&lt;= ");
  boost::replace_all(sourceCodeTxt, ">=", "&gt;= ");

  return sourceCodeTxt;
}

void GenerateAutomataTruePass::hasCallOnBasicBlock(BasicBlock& B,
                                                   LLVMContext* Ctx) {
  // if(this->currentFunction->getName() ==  "main")
  //{

  for (BasicBlock::iterator i = B.begin(), ie = B.end(); i != ie; ++i) {
    // for(auto& I:B)
    //{
    if (auto* cI = dyn_cast<CallInst>(BBIteratorToInst(i))) {
      // avoid bug if call pointer functions
      if (!cI->getCalledValue()->getName().empty()) {
        Value* v = cI->getCalledValue();

        Function* calleeFunction = dyn_cast<Function>(v->stripPointerCasts());

        if (calleeFunction->getName() != "__VERIFIER_assume" &&
            calleeFunction->getName() != "__VERIFIER_nondet_int" &&
            calleeFunction->getName() != "__VERIFIER_nondet_char" &&
            calleeFunction->getName() != "__VERIFIER_nondet_pointer" &&
            calleeFunction->getName() != "__VERIFIER_nondet_long" &&
            calleeFunction->getName() != "__VERIFIER_nondet_ushort" &&
            calleeFunction->getName() != "map2check_assume" &&
            calleeFunction->getName() != "malloc" &&
            calleeFunction->getName() != "calloc" &&
            calleeFunction->getName() != "realloc" &&
            calleeFunction->getName() != "free") {
          // BasicBlock::iterator iI= &;
          // i->dump();
          DebugInfo debugInfoFi(this->Ctx, BBIteratorToInst(i));
          this->st_numLineBeginBB = debugInfoFi.getLineNumberInt();
          this->st_isControl = false;
          this->st_isEntryPoint = true;
          this->countEntryPoint++;
          // this->st_sourceCodeLine =
          // this->sourceCodeHelper->getLine(debugInfoFi.getLineNumberInt());
          this->st_sourceCodeLine = this->replaceCodeByXml(
              this->sourceCodeHelper->getLine(debugInfoFi.getLineNumberInt()));
          this->st_startline = debugInfoFi.getLineNumberInt();

          this->printStateData();
        }
      }
    }
  }
  //}
}

void GenerateAutomataTruePass::printStateData() {
  if (this->enableDataBlk) {
    ofstream filest;
    filest.open("automata_list_log.st", std::ios_base::app);
    filest << this->st_currentFunctionName << "@";
    filest << this->st_numLineBeginBB << "@";
    filest << this->st_startline << "@";
    filest << this->st_sourceCodeLine << "@";
    if (this->st_isControl && this->numLineControlSt == this->st_startline) {
      filest << this->st_controlCode << "@1@";
      filest << this->st_numLineGoControl_1true << "@";
      filest << this->st_numLineGoControl_2false << "@";
    } else {
      filest << "NONE@0@NONE@NONE@";
    }
    filest << this->st_isEntryPoint << "@";
    filest << this->st_isErrorLocation << "\n";
    filest.close();
  }

  // To write the automata the flow should be using the function name and its
  // call
}

void GenerateAutomataTruePass::runOnBasicBlock(BasicBlock& B,
                                               LLVMContext* Ctx) {
  this->actualSizeBB = B.size();
  this->firstBlockInst = B.begin();
  this->st_lastBlockInst =
      --B.end();  // -- is necessary to avoid the pointer to the next block
  this->enableDataBlk = false;
  bool isCond = false;
  this->numLineControlSt = 0;

  // Getting data from BB begin
  DebugInfo debugInfoFi(this->Ctx, BBIteratorToInst(this->firstBlockInst));
  this->st_numLineBeginBB = debugInfoFi.getLineNumberInt();

  // Identifying asserts on analayzed code
  this->identifyAssertLoc(B);

  // if(!this->checkBBHasLError(B))
  //{
  //}
  this->checkBBHasLError(B);
  // Create a method to get info from branches in the block for condition-true
  // and false
  isCond = this->isBranchCond(B);
  this->st_isControl = isCond;

  if (B.size() > 1) {
    if (auto* tI = dyn_cast<TerminatorInst>(&*this->st_lastBlockInst)) {
      if (std::string(tI->getOpcodeName()) == "br") {
        --this->st_lastBlockInst;
        this->checkAndSkipAssume();
        // DebugInfo debugInfoLa(this->Ctx,
        // (Instruction*)this->st_lastBlockInst); this->st_startline =
        // debugInfoLa.getLineNumberInt(); this->st_sourceCodeLine =
        // this->sourceCodeHelper->getLine(debugInfoLa.getLineNumberInt());

      } else {
        DebugInfo debugInfoLa(this->Ctx,
                              BBIteratorToInst(this->st_lastBlockInst));

        if (this->sourceCodeHelper->getLine(debugInfoLa.getLineNumberInt())
                .empty()) {
          // int numline = debugInfoLa.getLineNumberInt() - 1;
          // this->st_startline = numline;
          // this->st_sourceCodeLine = this->sourceCodeHelper->getLine(numline);
          this->skipEmptyLine();

        } else {
          // this->st_startline = debugInfoLa.getLineNumberInt();
          // this->st_sourceCodeLine =
          // this->sourceCodeHelper->getLine(debugInfoLa.getLineNumberInt());
          this->checkAndSkipAssume();
        }
      }
    }
    this->enableDataBlk = true;

  } else {
    // In case this unique instruction be a "br" then we remove this basic block
    if (auto* tI = dyn_cast<TerminatorInst>(&*this->st_lastBlockInst)) {
      if (std::string(tI->getOpcodeName()) != "br") {
        this->st_lastBlockInst = this->firstBlockInst;

        // DebugInfo debugInfoLa(this->Ctx,
        // (Instruction*)this->st_lastBlockInst); this->st_startline =
        // debugInfoLa.getLineNumberInt();
        // this->st_sourceCodeLine =
        // this->sourceCodeHelper->getLine(debugInfoLa.getLineNumberInt());
        this->checkAndSkipAssume();

        this->enableDataBlk = true;
      }
    }
  }

  if (this->enableDataBlk) {
    // this->visitInstruction(i);
    if (this->currentFunction->getName() == "main") {
      if (this->countEntryPoint == 1 && this->st_isEntryPoint == false) {
        this->st_isEntryPoint = true;
        this->countEntryPoint++;
      } else {
        this->st_isEntryPoint = false;
      }
    }
  }
}

bool GenerateAutomataTruePass::checkInstBbIsAssume(BasicBlock::iterator& iT) {
  if (auto* cI = dyn_cast<CallInst>(BBIteratorToInst(iT))) {
    if (!cI->getCalledValue()->getName().empty()) {
      Value* v = cI->getCalledValue();
      Function* calleeFunction = dyn_cast<Function>(v->stripPointerCasts());

      if (calleeFunction->getName() == "__VERIFIER_assume" ||
          calleeFunction->getName() == "map2check_assume") {
        return true;
      } else {
        return false;
      }
    }
  }
  return false;
}

// Checking if the instruction is an ASSUME to skip
void GenerateAutomataTruePass::checkAndSkipAssume() {
  DebugInfo debugInfoLa(this->Ctx, BBIteratorToInst(this->st_lastBlockInst));
  this->numLineBlk_ori = debugInfoLa.getLineNumberInt();
  bool flagAssume = false;

  if (this->checkInstBbIsAssume(this->st_lastBlockInst)) {
    flagAssume = true;
  } else {
    this->st_startline = this->numLineBlk_ori;
    // this->st_sourceCodeLine =
    // this->sourceCodeHelper->getLine(this->numLineBlk_ori);
    this->st_sourceCodeLine = this->replaceCodeByXml(
        this->sourceCodeHelper->getLine(this->numLineBlk_ori));
  }

  while (flagAssume) {
    --this->st_lastBlockInst;
    DebugInfo debugInfoAa(this->Ctx, BBIteratorToInst(this->st_lastBlockInst));
    this->numLineBlk_AA = debugInfoAa.getLineNumberInt();
    if (this->numLineBlk_AA == this->numLineBlk_ori) {
      flagAssume = true;
    } else {
      flagAssume = false;
      if (this->sourceCodeHelper->getLine(this->numLineBlk_AA).empty()) {
        this->skipEmptyLine();
      } else {
        this->st_startline = this->numLineBlk_AA;
        // this->st_sourceCodeLine =
        // this->sourceCodeHelper->getLine(this->numLineBlk_AA);
        this->st_sourceCodeLine = this->replaceCodeByXml(
            this->sourceCodeHelper->getLine(this->numLineBlk_AA));
      }
    }
  }
}

void GenerateAutomataTruePass::skipEmptyLine() {
  bool flagEmpty = true;
  int countInstBlk = 1;
  bool lastInst = false;
  while (flagEmpty) {
    // this->st_lastBlockInst->dump();

    // errs() << this->actualSizeBB << "\n";
    if (countInstBlk < this->actualSizeBB) {
      --this->st_lastBlockInst;
      countInstBlk++;
    } else {
      lastInst = true;
    }

    DebugInfo debugInfoAaEmptyW(this->Ctx,
                                BBIteratorToInst(this->st_lastBlockInst));
    if (this->sourceCodeHelper->getLine(debugInfoAaEmptyW.getLineNumberInt())
            .empty()) {
      if (!lastInst) {
        flagEmpty = true;
      } else {
        flagEmpty = false;
        this->st_startline = debugInfoAaEmptyW.getLineNumberInt();
        // errs() << this->st_startline << "\n";
        // this->st_sourceCodeLine =
        // this->sourceCodeHelper->getLine(debugInfoAaEmptyW.getLineNumberInt());
        this->st_sourceCodeLine =
            this->replaceCodeByXml(this->sourceCodeHelper->getLine(
                debugInfoAaEmptyW.getLineNumberInt()));
      }
    } else {
      flagEmpty = false;
      this->st_startline = debugInfoAaEmptyW.getLineNumberInt();
      // this->st_sourceCodeLine =
      // this->sourceCodeHelper->getLine(debugInfoAaEmptyW.getLineNumberInt());
      this->st_sourceCodeLine =
          this->replaceCodeByXml(this->sourceCodeHelper->getLine(
              debugInfoAaEmptyW.getLineNumberInt()));
    }
  }
}

void GenerateAutomataTruePass::identifyAssertLoc(BasicBlock& B) {
  for (auto& I : B) {
    if (auto* cI = dyn_cast<CallInst>(&I)) {
      if (!cI->getCalledValue()->getName().empty()) {
        Value* v = cI->getCalledValue();
        Function* calleeFunction = dyn_cast<Function>(v->stripPointerCasts());
        // errs() << calleeFunction->getName() << "\n";
        if (calleeFunction->getName() == "__VERIFIER_assert") {
          DebugInfo debugInfoCi(this->Ctx, cI);
          this->assertListLoc.push_back(debugInfoCi.getLineNumberInt());
        }
      }
    }
  }
}

BasicBlock::iterator& GenerateAutomataTruePass::skipEmptyLineIt(
    BasicBlock::iterator& iT) {
  bool flagEmpty = true;
  int countInst = 1;
  while (flagEmpty) {
    DebugInfo debugInfoAaEmptyW(this->Ctx, BBIteratorToInst(iT));
    bool skip = false;
    // errs() <<
    // this->sourceCodeHelper->getLine(debugInfoAaEmptyW.getLineNumberInt()) <<
    // "<<<<<<<<<<<< \n";

    if (this->sourceCodeHelper->getLine(debugInfoAaEmptyW.getLineNumberInt())
            .empty() ||
        this->sourceCodeHelper->getLine(debugInfoAaEmptyW.getLineNumberInt()) ==
            "}") {
      skip = true;
    }

    if (std::string(iT->getOpcodeName()) == "br" || skip) {
      // errs() << "HERE \n";
      // errs() <<countInst << "<" << this->actualSizeBB << "\n";
      if (countInst < this->skipEmptyLineItSize) {
        --iT;
        countInst++;
        flagEmpty = true;
      } else {
        flagEmpty = false;
      }

    } else {
      flagEmpty = false;
    }
  }

  return iT;
}

// Identify if the block has a branch and define the condition to true and false
bool GenerateAutomataTruePass::isBranchCond(BasicBlock& B) {
  // errs() << bbName << "\n";

  std::vector<int>::const_iterator iT;
  bool retResult = false;

  for (auto& I : B) {
    if (auto* bI = dyn_cast<ICmpInst>(&I)) {
      // bI->dump();
      // errs() << this->convertLLPredicateToXmlText(I) << "\n";
      //
      DebugInfo debugInfoBi(this->Ctx, bI);
      // errs() << debugInfoBi.getLineNumberInt() << "\n";

      // Skiping when the Branch was an assertion
      iT = std::find(this->assertListLoc.begin(), this->assertListLoc.end(),
                     debugInfoBi.getLineNumberInt());
      if (!(iT != this->assertListLoc.end())) {
        // errs() << "control : " << this->convertLLPredicateToXmlText(I) <<
        // "\n";
        this->st_controlCode = this->convertLLPredicateToXmlText(I);
        this->numLineControlSt = debugInfoBi.getLineNumberInt();

        // getting true and false linenum from bb

        retResult = true;
      }
    }

    if (auto* tI = dyn_cast<TerminatorInst>(&I)) {
      if (std::string(tI->getOpcodeName()) == "br") {
        // tI->dump();
        if (tI->getNumSuccessors() > 1) {
          // TRUE cond
          this->skipEmptyLineItSize = tI->getSuccessor(0)->size();
          BasicBlock::iterator trueCond = --tI->getSuccessor(0)->end();
          if (std::string(trueCond->getOpcodeName()) == "br" &&
              tI->getSuccessor(0)->size() == 1) {
            trueCond = --tI->getSuccessor(0)->getSingleSuccessor()->end();
          }

          if (std::string(trueCond->getOpcodeName()) == "br" ||
              std::string(trueCond->getOpcodeName()) == "ret") {
            trueCond = this->skipEmptyLineIt(trueCond);
          }

          // errs() << "TRUE \n";
          // trueCond->dump();

          DebugInfo debugInfoT(this->Ctx, BBIteratorToInst(trueCond));
          this->st_numLineGoControl_1true = debugInfoT.getLineNumberInt();

          // FALSE cond
          // tI->getSuccessor(1)->dump();
          // tI->getSuccessor(1)->begin()->dump();
          BasicBlock::iterator falseCond;
          this->skipEmptyLineItSize = tI->getSuccessor(1)->size();
          if (this->actualSizeBB > 1) {
            falseCond = --tI->getSuccessor(1)->end();
          } else {
            falseCond = tI->getSuccessor(1)->begin();
          }

          // falseCond->dump();

          if (std::string(falseCond->getOpcodeName()) == "br" &&
              tI->getSuccessor(1)->size() == 1) {
            falseCond = --tI->getSuccessor(1)->getSingleSuccessor()->end();
          }

          if (std::string(falseCond->getOpcodeName()) == "br" ||
              std::string(falseCond->getOpcodeName()) == "ret") {
            falseCond = this->skipEmptyLineIt(falseCond);
          }

          // falseCond->dump();
          DebugInfo debugInfoF(this->Ctx, BBIteratorToInst(falseCond));
          this->st_numLineGoControl_2false = debugInfoF.getLineNumberInt();
          // errs() << this->st_numLineGoControl_2false << " --- \n";
        }
      }
    }
  }

  if (retResult) {
    return true;
  } else {
    return false;
  }
}

std::string GenerateAutomataTruePass::convertLLPredicateToXmlText(
    Instruction& I) {
  std::string lvaluep;
  std::string rvaluep;
  std::string predicateInXml;
  std::string fullExpPredicateInXml;
  std::ostringstream osstrtmp;

  if (auto* bI = dyn_cast<ICmpInst>(&I)) {
    // bI->dump();
    // errs() << bI->getSignedPredicate() << "\n";
    // errs() << this->getPredicateSymOnXmlText(*bI) << "\n";
    predicateInXml = this->getPredicateSymOnXmlText(*bI);
    // errs() << *bI->getOperand(0) << "\n";
    // errs() << bI->getOperand(0)->getValueName() << "\n";

    // Getting left value from predicate
    if (isa<LoadInst>(bI->getOperand(0))) {
      LoadInst* LD100 = cast<LoadInst>(bI->getOperand(0));
      Value* C100 = LD100->getPointerOperand();
      lvaluep = C100->getName().str();
      // errs() << "LVALUEP: " << lvaluep << "\n";

    } else if (ConstantInt* CI = dyn_cast<ConstantInt>(bI->getOperand(0))) {
      if (CI->getBitWidth() <=
          32) {  // Of course, you can also change it to <= 64 if constIntValue
                 // is a 64-bit integer, etc.
        // constIntValue = CI->getSExtValue();
        osstrtmp << CI->getSExtValue();
        rvaluep = osstrtmp.str();
        // errs() << "LVALUEP:" << lvaluep << "\n";
      }
    }

    // Instruction tmpI = (Instruction) &*bI->getOperand(0);
    // errs() << *bI->getOperand(1) << "\n";
    // Getting right value from predicate
    if (isa<LoadInst>(bI->getOperand(1))) {
      LoadInst* LD101 = cast<LoadInst>(bI->getOperand(1));
      Value* C101 = LD101->getPointerOperand();  // HERE COMPILATION ERROR
      rvaluep = C101->getName().str();
      // errs() << "RVALUEP:" << rvaluep << "\n";
    } else if (ConstantInt* CI = dyn_cast<ConstantInt>(bI->getOperand(1))) {
      // errs() << bI->getOperand(1)->getName().str() << "\n";
      // Of course, you can also change it to <= 64 if constIntValue
      // is a 64-bit integer, etc.
      // constIntValue = CI->getSExtValue();
      if (CI->getBitWidth() <= 32) {
        osstrtmp << CI->getSExtValue();
        rvaluep = osstrtmp.str();
        // errs() << "RVALUEP:" << rvaluep << "\n";
      }
    }
    //}

    // Generate full predicate expression
    // errs() << lvaluep.empty() << "==\n";
    // errs() << rvaluep.empty() << "==\n";
    // errs() << "[" + lvaluep + " " + predicateInXml + " " + rvaluep + "]" <<
    // "\n";
    if (this->isPredicateNe && lvaluep.empty()) {
      fullExpPredicateInXml = "[ !" + rvaluep + "]";
    } else if (this->isPredicateNe && rvaluep.empty()) {
      fullExpPredicateInXml = "[ !" + lvaluep + "]";
    } else {
      fullExpPredicateInXml =
          "[" + lvaluep + " " + predicateInXml + " " + rvaluep + "]";
    }
    // errs() << fullExpPredicateInXml << "\n";
    return fullExpPredicateInXml;
  }
  return "[]";
}

std::string GenerateAutomataTruePass::getPredicateSymOnXmlText(
    ICmpInst& icmpInst) {
  std::string predicateText = "";
  this->isPredicateNe = false;

  ICmpInst::Predicate pr = icmpInst.getSignedPredicate();
  if (pr == ICmpInst::ICMP_EQ) {
    predicateText = "==";
  } else if (pr == ICmpInst::ICMP_NE) {
    predicateText = "!=";
    this->isPredicateNe = true;
  } else if (pr == ICmpInst::ICMP_SGT) {
    predicateText = "&gt;";
  } else if (pr == ICmpInst::ICMP_SGE) {
    predicateText = "&gt;=";
  } else if (pr == ICmpInst::ICMP_SLT) {
    predicateText = "&lt;";
  } else if (pr == ICmpInst::ICMP_SLE) {
    predicateText = "&lt;=";
  }
  /**switch(pr){
      case ICmpInst::ICMP_SGT: predicateText="&gt;"; break;
      case ICmpInst::ICMP_SLT: predicateText="&lt;"; break;
      case ICmpInst::ICMP_SGE: predicateText="&ge;"; break;
      case ICmpInst::ICMP_SLE: predicateText="&le;"; break;
  }**/

  return predicateText;
}

// To identify a block with a error location by __VERIFIER_error call function
bool GenerateAutomataTruePass::checkBBHasLError(BasicBlock& nowB) {
  this->st_isErrorLocation = 0;
  for (auto& I : nowB) {
    if (CallInst* callInst = dyn_cast<CallInst>(&I)) {
      if (!callInst->getCalledValue()->getName().empty()) {
        // errs() << callInst->getCalledFunction()->getName() << "\n";
        Value* v = callInst->getCalledValue();
        Function* calleeFunction = dyn_cast<Function>(v->stripPointerCasts());
        if (calleeFunction->getName() == "__VERIFIER_error") {
          this->st_isErrorLocation = 1;
          return true;
        }
      }
    }
  }
  // std::cout << i.begin << "\n";
  // std::cout << i->getOpcodeName(i->getOpcode()) << std::endl;
  return false;
}

char GenerateAutomataTruePass::ID = 20;
static RegisterPass<GenerateAutomataTruePass> X("generate_automata_true",
                                                "Generate Automata True");
