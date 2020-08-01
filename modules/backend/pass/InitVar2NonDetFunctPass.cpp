/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * LLVM -> NCSA
 *
 * SPDX-License-Identifier: (GPL-2.0 AND NCSA)
 **/

#include "InitVar2NonDetFunctPass.hpp"

using llvm::dyn_cast;
using llvm::errs;
using llvm::IRBuilder;
using llvm::RegisterPass;

bool InitVar2NonDetFunctionPass::runOnFunction(Function &F) {
  bool flag_modified = false;
  int id_tmp = 0;

  // Look for main function
  if (F.getName() == "main") {
    LLVMContext &Ctx = F.getContext();
    llvm::Constant *funct;

    // Running each basic block
    for (auto &bb : F) {
      // Looking for the instructions
      for (auto &instruction : bb) {
        // For each Alloca instructions one Store should be
        // found, if not that Alloca was not initialized
        if (auto *al = dyn_cast<llvm::AllocaInst>(&instruction)) {
          // This because of backtracking def-use chain searching
          // https://stackoverflow.com/questions/35370195/llvm-difference-between-uses-and-user-in-instruction-or-value-classes
          if (al->getNumUses() > 0) {
            //al->reverseUseList();

            // We need the first use from instruction
            // to check if is an store instruction
            if (auto *inst =
                    dyn_cast<llvm::LoadInst>(al->uses().begin()->getUser())) {
              
              // Now we create a callinst instruction
              // to be inserted before this instruction
              // Get the basic block from LoadInst
              BasicBlock * actual_bb = inst->getParent();
              IRBuilder<> builder(reinterpret_cast<Instruction *>(inst));
              builder.SetInsertPoint(actual_bb, builder.GetInsertPoint());

              // Identify the variable type
              if (al->getAllocatedType()->isIntegerTy()) {
                // errs() << *al << "\n";
                funct = F.getParent()->getOrInsertFunction(
                    "map2check_non_det_int", Type::getInt32Ty(Ctx));
                Function *hook = dyn_cast<Function>(funct);
                //std::string name_tmp_var = "tmp_call_" + std::to_string(id_tmp);

                // Create Call Inst to __VERIFIER_nondet_*
                auto *cInst = builder.CreateCall(hook, llvm::None,
                                                 "", NULL); //name_tmp_var.c_str()

                // Create a Load instruction
                // Value *cast = builder.CreatePtrToInt(cInst, Type::getInt32Ty(Ctx)); \
                // builder.CreateLoad(Type::getInt32PtrTy(Ctx), cast);
                
                // Create a store instruction
                builder.SetInsertPoint(actual_bb, builder.GetInsertPoint());
                builder.CreateStore(cInst, al);

                flag_modified = true;
              }
            }
          }
        }
      }
    }
  }

  // errs() << flag_modified << "\n";

  if (flag_modified) {
    return true;
  } else {
    // The IR was not modified
    return false;
  }
}

char InitVar2NonDetFunctionPass::ID = 15;
static RegisterPass<InitVar2NonDetFunctionPass> X(
    "init_var2_nondet",
    "Identify variable not initialized (only in the main funct) and then apply "
    "a nondet function using __VERIFIER_nondet_type() call");
