/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#include "TrackBasicBlockPass.hpp"

#include <memory>
#include <string>

using llvm::CallInst;
using llvm::dyn_cast;
using llvm::IRBuilder;
using llvm::isa;
using llvm::PHINode;
using llvm::RegisterPass;
// using llvm::TerminatorInst;
using llvm::Instruction;
// using llvm::Instruction::isTerminator;
using llvm::UnreachableInst;

namespace {
inline Instruction* BBIteratorToInst(BasicBlock::iterator i) {
  Instruction* pointer = reinterpret_cast<Instruction*>(&*i);
  return pointer;
}
}  // namespace

bool TrackBasicBlockPass::runOnFunction(Function& F) {
  this->Ctx = &F.getContext();
  this->currentFunction = &F;
  this->libraryFunctions = make_unique<LibraryFunctions>(&F, &F.getContext());

  int countBB = 1;
  for (auto& B : F) {
    // this->instrumentEntryBB(B, this->Ctx);
    // if(countBB == 1)
    //{
    this->hasCallOnBasicBlock(B, this->Ctx);
    //}
    this->runOnBasicBlock(B, this->Ctx);
    countBB++;
  }

  return true;
}
/**
void TrackBasicBlockPass::instrumentEntryBB(BasicBlock& B, LLVMContext* Ctx)
{
        BasicBlock::iterator firstInst = B.begin();
        //firstInst++;
        DebugInfo debugInfoFi(this->Ctx, (Instruction*)firstInst);
        unsigned fiLineNum = 0;

        if(B.size() > 1 &&
this->sourceCodeHelper->getLine(debugInfoFi.getLineNumberInt()).empty()){

                bool flagEmpty = false;
                int countInst = 1;
                DebugInfo debugInfoFiEmpty(this->Ctx, (Instruction*)firstInst);
                if(this->sourceCodeHelper->getLine(debugInfoFiEmpty.getLineNumberInt()).empty())
                {
                        flagEmpty = true;
                }

                while(flagEmpty)
                {
                        if(countInst < B.size())
                        {
                                countInst++;
                                firstInst++;
                                DebugInfo debugInfoFiEmptyW(this->Ctx,
(Instruction*)firstInst); fiLineNum = debugInfoFiEmptyW.getLineNumberInt();
                                if(this->sourceCodeHelper->getLine(debugInfoFiEmptyW.getLineNumberInt()).empty())
                                {
                                        flagEmpty = true;
                                }else{
                                        flagEmpty = false;
                                }
                        }else{
                                flagEmpty = false;
                        }

                }
                //errs() << fiLineNum << "\n";
                //firstInst->dump();
                // Code instrumentation in the end of the BB
                this->instrumentInstBB(firstInst);

        }else{
                //errs() << debugInfoFi.getLineNumberInt() << "\n";
                //firstInst->dump();
                this->instrumentInstBB(firstInst);
        }

}**/

void TrackBasicBlockPass::hasCallOnBasicBlock(BasicBlock& B, LLVMContext* Ctx) {
  // if(this->currentFunction->getName() ==  "main")
  //{

  for (BasicBlock::iterator i = B.begin(), ie = B.end(); i != ie; ++i) {
    // for(auto& I:B)
    //{
    if (auto* cI = dyn_cast<CallInst>(BBIteratorToInst(i))) {
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
          IRBuilder<> builder(BBIteratorToInst(i));
          this->functionName =
              builder.CreateGlobalStringPtr(this->currentFunction->getName());
          // i--;
          this->instrumentInstBB(i);
          // i++;
        }
      }
    }
  }
  //}
}

void TrackBasicBlockPass::runOnBasicBlock(BasicBlock& B, LLVMContext* Ctx) {
  this->st_lastBlockInst =
      --B.end();  // -- is necessary to avoid the pointer to the next block

  /**if(!this->initializedFunctionName) {
        IRBuilder<> builder((Instruction*)&*this->st_lastBlockInst);
        this->functionName = builder
                        .CreateGlobalStringPtr(this->currentFunction->getName());
        this->initializedFunctionName = true;
  }**/

  IRBuilder<> builder(reinterpret_cast<Instruction*>(&*this->st_lastBlockInst));
  this->functionName =
      builder.CreateGlobalStringPtr(this->currentFunction->getName());

  // errs() << this->currentFunction->getName() << "\n";
  // errs() << this->st_lastBlockInst->getOpcodeName() << "\n";
  // DebugInfo debugInfoLa(this->Ctx, (Instruction*)this->st_lastBlockInst);
  // errs() << debugInfoLa.getLineNumberInt() << "\n";

  // if (auto* tI = dyn_cast<TerminatorInst>(&*this->st_lastBlockInst)) {
  if ( this->st_lastBlockInst->isTerminator() ) {
    if (std::string(this->st_lastBlockInst->getOpcodeName()) == "br") {
      if (B.size() > 1) {
        --this->st_lastBlockInst;
        DebugInfo debugInfoLa(this->Ctx,
                              (BBIteratorToInst(this->st_lastBlockInst)));
        this->numLineBlk_ori = debugInfoLa.getLineNumberInt();
        bool flagAssume = false;
        // Checking if the instruction is an ASSUME to skip
        if (this->checkInstBbIsAssume(this->st_lastBlockInst)) {
          flagAssume = true;
        }

        while (flagAssume) {
          --this->st_lastBlockInst;
          DebugInfo debugInfoAa(this->Ctx,
                                BBIteratorToInst(this->st_lastBlockInst));
          this->numLineBlk_AA = debugInfoAa.getLineNumberInt();
          // errs() << this->numLineBlk_AA << "  " << this->numLineBlk_ori <<
          // "\n";
          if (this->numLineBlk_AA == this->numLineBlk_ori &&
              !this->isUnreachableInst) {
            flagAssume = true;
          } else {
            flagAssume = false;
          }
        }

        // errs() << this->numLineBlk_AA << " 1 \n";
        this->instrumentInstBB(this->st_lastBlockInst);
      }

    } else {
      // this->st_lastBlockInst->dump();

      DebugInfo debugInfoLa(this->Ctx,
                            BBIteratorToInst(this->st_lastBlockInst));
      this->numLineBlk_ori = debugInfoLa.getLineNumberInt();
      bool flagAssume = false;
      // Checking if the instruction is an ASSUME to skip
      if (this->checkInstBbIsAssume(this->st_lastBlockInst)) {
        flagAssume = true;
      }

      while (flagAssume) {
        --this->st_lastBlockInst;
        DebugInfo debugInfoAa(this->Ctx,
                              BBIteratorToInst(this->st_lastBlockInst));
        this->numLineBlk_AA = debugInfoAa.getLineNumberInt();
        // errs() << this->numLineBlk_AA << "  " << this->numLineBlk_ori <<
        // "\n";
        if (this->numLineBlk_AA == this->numLineBlk_ori &&
            !this->isUnreachableInst) {
          flagAssume = true;
        } else {
          flagAssume = false;
        }
      }

      this->numLineBlk_AA = debugInfoLa.getLineNumberInt();

      // To avoid empty lines, e.g., only with "}"
      bool flagEmpty = false;
      unsigned countInst = 1;
      DebugInfo debugInfoAaEmpty(this->Ctx,
                                 BBIteratorToInst(this->st_lastBlockInst));
      if (this->sourceCodeHelper->getLine(debugInfoAaEmpty.getLineNumberInt())
              .empty()) {
        flagEmpty = true;
      }

      while (flagEmpty) {
        if (countInst < B.size()) {
          countInst++;
          --this->st_lastBlockInst;
          DebugInfo debugInfoAaEmptyW(this->Ctx,
                                      BBIteratorToInst(this->st_lastBlockInst));
          if (this->sourceCodeHelper
                  ->getLine(debugInfoAaEmptyW.getLineNumberInt())
                  .empty()) {
            flagEmpty = true;
          } else {
            flagEmpty = false;
          }
        } else {
          flagEmpty = false;
        }
      }

      // Code instrumentation in the end of the BB
      this->instrumentInstBB(this->st_lastBlockInst);
    }
  }
}

bool TrackBasicBlockPass::checkInstBbIsAssume(BasicBlock::iterator& iT) {
  // iT->dump();
  // errs() << iT->getOpcodeName() << " \n";
  this->isUnreachableInst = false;
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

  } else if (dyn_cast<UnreachableInst>(BBIteratorToInst(iT)) != NULL) {
    this->isUnreachableInst = true;
    return true;
  }
  return false;
}

void TrackBasicBlockPass::instrumentInstBB(BasicBlock::iterator& iT) {
  /**auto j = iT;

  errs() << iT->getOpcodeName() << "\n";
  if(iT->getOpcodeName() == "ret")
  {
          ++j;
  }else{
          --j;
  }**/

  // iT->dump();
  // To avoid that PHI nodes not grouped at top of basic block
  if (isa<PHINode>(iT)) {
    // errs() << "FOUND PHINODE \n";
    iT++;
  }

  // Twine track_bb("map2check_track_bb");
  // Function *calleeFunction;
  // calleeFunction->setName(track_bb);
  // errs() << "inst 1 \n";

  IRBuilder<> builder(BBIteratorToInst(iT));
  // errs() << "inst 2 \n";
  Value* function_llvm = this->getFunctionNameValue();

  DebugInfo debugInfo(this->Ctx, BBIteratorToInst(iT));
  // errs() << *debugInfo.getLineNumberValue() << "\n";
  // errs() << function_llvm->getName() << "\n";

  Value* args[] = {debugInfo.getLineNumberValue(), function_llvm};

  builder.CreateCall(this->libraryFunctions->getTrackBBFunction(), args);
  // errs() << "inst 3 \n";
}

char TrackBasicBlockPass::ID = 12;
static RegisterPass<TrackBasicBlockPass> X(
    "track_basic_block", "Track Basic Block in the symbolic execution");
