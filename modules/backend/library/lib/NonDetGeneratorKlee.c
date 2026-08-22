/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include "../header/Map2CheckFunctions.h"
#include "../header/NonDetGenerator.h"
#include "../header/NonDetLog.h"
#include <stdlib.h>

extern int __map2check_main__(int argc, char **argv);

int main(int argc, char **argv) { return __map2check_main__(argc, argv); }

void nondet_init() { nondet_log_init(); }
void nondet_destroy() { nondet_log_destroy(); }
void nondet_generate_aux_witness_files() {
  nondet_log_to_file(map2check_nondet_get_log());
}

extern void klee_assume(int);

void nondet_assume(int expr) { klee_assume(expr); }

extern void klee_make_symbolic(void *addr, size_t nbytes, const char *name);

/* "non_det_" #type, not "non_det_#type". The stringify operator only applies
 * to a macro parameter written OUTSIDE a string literal; inside one, # and t
 * are just two characters. Every symbolic object was therefore named the
 * literal text "non_det_#type" -- the same for an int, a double and a char.
 *
 * Nothing read that name, which is how it survived: the nondet log carries its
 * own type column. It matters now because KLEE writes one .ktest per explored
 * path, holding exactly the input vectors a Cover-Branches suite needs, and a
 * .ktest records each object's name and size but not its type. With the name
 * fixed the type travels with the object; without it, four bytes could be an
 * int, a float or half of a long, and nothing tells them apart. */
#define MAP2CHECK_NON_DET_GENERATOR(type)                                      \
  type map2check_non_det_##type() {                                            \
    type non_det;                                                              \
    klee_make_symbolic(&non_det, sizeof(non_det), "non_det_" #type);           \
    return non_det;                                                            \
  }

char *map2check_non_det_pchar() {
  unsigned length = map2check_non_det_unsigned();
  if (length == 0)
    return NULL;
  /* heap allocation: returning a local VLA would leave the caller with a
   * dangling pointer (cppcheck returnDanglingLifetime) */
  char *string = malloc(length);
  if (string == NULL)
    return NULL;
  unsigned i = 0;
  for (i = 0; i < (length - 1); i++) {
    string[i] = map2check_non_det_char();
  }
  string[i] = '\0';
  return string;
}

// Using default generator for:
MAP2CHECK_NON_DET_GENERATOR(int)
MAP2CHECK_NON_DET_GENERATOR(char)
MAP2CHECK_NON_DET_GENERATOR(pointer)
MAP2CHECK_NON_DET_GENERATOR(ushort)
MAP2CHECK_NON_DET_GENERATOR(short)
MAP2CHECK_NON_DET_GENERATOR(long)
MAP2CHECK_NON_DET_GENERATOR(unsigned)
MAP2CHECK_NON_DET_GENERATOR(ulong)
MAP2CHECK_NON_DET_GENERATOR(bool)
MAP2CHECK_NON_DET_GENERATOR(uchar)
MAP2CHECK_NON_DET_GENERATOR(size_t)
#ifndef __INTELLISENSE__
MAP2CHECK_NON_DET_GENERATOR(loff_t)
#endif
MAP2CHECK_NON_DET_GENERATOR(sector_t)
MAP2CHECK_NON_DET_GENERATOR(uint)
MAP2CHECK_NON_DET_GENERATOR(double)
