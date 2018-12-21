/**********************************************************************
 * This module contains classes to make ScopeTracking
 ***********************************************************************/

#pragma once
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/Support/raw_ostream.h>
#include <map>

namespace {
unsigned ID_COUNTER;

std::atomic<unsigned> id;

unsigned create_id() { return ID_COUNTER++; }
}  // namespace

/** Data Structure responsible to holding id and parent of scope */
struct ScopeObj {
  unsigned id;     /**< ID of Scope */
  unsigned parent; /**< Outter Scope of ID, if 0 it means root*/
};

/** Class responsible to Track and Update all Scopes */
class ScopeMap {
 private:
  std::map<llvm::DIScopeRef, ScopeObj> referencesMap;
  ScopeObj getOrInsert(llvm::DIScopeRef);
  void updateReference(llvm::DIScopeRef, ScopeObj);
  void addParentScope(llvm::DILocalVariable *);
  void addParentScope(llvm::DIScopeRef, llvm::DILexicalBlock *);
  void setParent(llvm::DIScopeRef, ScopeObj, ScopeObj);

 public:
  ScopeMap() {}
  ScopeObj get(llvm::DIScopeRef ref) { return this->getOrInsert(ref); }
  ScopeObj get(llvm::DILocalVariable *);
  void dump();
};

ScopeObj ScopeMap::getOrInsert(llvm::DIScopeRef ref) {
  if (referencesMap.find(ref) == referencesMap.end()) {
    ScopeObj result;
    result.id = create_id();
    result.parent = 0;
    referencesMap[ref] = result;
  }
  return referencesMap[ref];
}

void ScopeMap::dump() {
  llvm::errs() << "\t*ScopeMap Dump*\n";
  llvm::errs() << "----------------------------------------------------\n";

  for (const auto &mapPair : referencesMap) {
    ScopeObj obj = mapPair.second;
    llvm::errs() << "ID: " << obj.id << " | Parent: " << obj.parent << "\n";
    llvm::errs() << "----------------------------------------------------\n";
  }
}

void ScopeMap::updateReference(llvm::DIScopeRef ref, ScopeObj obj) {
  referencesMap[ref] = obj;
}

void ScopeMap::setParent(llvm::DIScopeRef ref, ScopeObj son, ScopeObj parent) {
  son.parent = parent.id;
  updateReference(ref, son);
}

ScopeObj ScopeMap::get(llvm::DILocalVariable *var) {
  addParentScope(var);
  return this->get(var->getScope()->getScope());
}

// If parent is root, return 0 or Function Scope (may be different)
void ScopeMap::addParentScope(llvm::DILocalVariable *var) {
  llvm::DILexicalBlock *lexical =
      llvm::dyn_cast<llvm::DILexicalBlock>(var->getScope());
  if (lexical == nullptr) {
    return;
  }

  llvm::DILexicalBlock *outter_lexical =
      llvm::dyn_cast<llvm::DILexicalBlock>(lexical->getRawScope());

  if (outter_lexical == nullptr) return;

  addParentScope(var->getScope()->getScope(), outter_lexical);
}
void ScopeMap::addParentScope(llvm::DIScopeRef ref,
                              llvm::DILexicalBlock *lexical) {
  ScopeObj result = this->get(ref);
  ScopeObj parent;
  // Add all intermediates scopes

  // Iterates overs all scopes until finds de sub program
  while (llvm::dyn_cast<llvm::DISubprogram>(lexical->getRawScope()) ==
         nullptr) {
    lexical = llvm::dyn_cast<llvm::DILexicalBlock>(lexical->getRawScope());
    parent = this->get(lexical->getScope());
    setParent(ref, result, parent);
    ref = lexical->getScope();
    result = parent;
  }

  llvm::DILexicalBlock *md_parent_scope =
      llvm::dyn_cast<llvm::DILexicalBlock>(lexical->getRawScope());

  if (md_parent_scope != nullptr) {
    parent = this->get(md_parent_scope->getScope());
    setParent(ref, result, parent);
  }
}