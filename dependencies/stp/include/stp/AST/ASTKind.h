#ifndef TESTKINDS_H
#define TESTKINDS_H
// Generated automatically by genkinds.pl from ASTKind.kinds.
// Do not edit
#include <iostream>
namespace stp {
  typedef enum {
  UNDEFINED,
  SYMBOL,
  BVCONST,
  BVNOT,
  BVCONCAT,
  BVOR,
  BVAND,
  BVXOR,
  BVNAND,
  BVNOR,
  BVXNOR,
  BVEXTRACT,
  BVLEFTSHIFT,
  BVRIGHTSHIFT,
  BVSRSHIFT,
  BVPLUS,
  BVSUB,
  BVUMINUS,
  BVMULT,
  BVDIV,
  BVMOD,
  SBVDIV,
  SBVREM,
  SBVMOD,
  BVSX,
  BVZX,
  ITE,
  BOOLEXTRACT,
  BVLT,
  BVLE,
  BVGT,
  BVGE,
  BVSLT,
  BVSLE,
  BVSGT,
  BVSGE,
  EQ,
  FALSE,
  TRUE,
  NOT,
  AND,
  OR,
  NAND,
  NOR,
  XOR,
  IFF,
  IMPLIES,
  PARAMBOOL,
  READ,
  WRITE,
  ARRAY,
  BITVECTOR,
  BOOLEAN
} Kind;

extern unsigned char _kind_categories[];

inline bool is_Term_kind(Kind k) { return (_kind_categories[k] & 1); }

inline bool is_Form_kind(Kind k) { return (_kind_categories[k] & 2); }

extern const char *_kind_names[];

/** Prints symbolic name of kind */
inline std::ostream& operator<<(std::ostream &os, const Kind &kind) { os << _kind_names[kind]; return os; }


}  // end namespace


#endif
