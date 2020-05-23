/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#include "CheckNonDetFunctPass.hpp"

#include <llvm/Support/raw_ostream.h> 

using llvm::dyn_cast;
using llvm::RegisterPass;
using llvm::errs;


bool CheckNonDetFunctPass::runOnFunction(Function& F) {

    // run on each basic block
    for(auto &BB : F){
        // run on each instruction from the basic block
        for(auto &I : BB){
            if(CallInst *callInst = dyn_cast<CallInst>(&I)){
                if(Function *calledFunct = callInst->getCalledFunction()){
                    if(calledFunct->getName().startswith("__VERIFIER_nondet")){
                        errs() << calledFunct->getName() << "\n";
                    }
                }
            }
        }
    }
    return false; // The IR was not modified
}


char CheckNonDetFunctPass::ID = 14;
static RegisterPass<CheckNonDetFunctPass> X("check_nondet_functs",
                                  "Identify nondet function using __VERIFIER_nondet_type() call");