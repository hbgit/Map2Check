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
#include <string.h>
#include <unistd.h>

extern int __map2check_main__(int argc, char **argv);

int main(int argc, char **argv) { return __map2check_main__(argc, argv); }

void nondet_init() { nondet_log_init(); }
void nondet_destroy() { nondet_log_destroy(); }
extern long long klee_get_value_i64(long long expr);
extern void klee_assume(int);

/* Pin every logged value to one concrete model before printing it. The values
 * are still symbolic when the path ends, and uClibc's vfprintf branches on
 * each digit it converts: every row multiplied the states, so a program with
 * ten nondet reads reached its target and never finished writing the log
 * (issue #75). The assume makes each value consistent with the path and with
 * the values pinned before it, so the row is one input vector, not a set of
 * independent guesses. The 64-bit view covers every member of the union; the
 * narrower ones were zero-extended by new_nondet_call's memset. */
static void concretize_nondet_log(MAP2CHECK_CONTAINER log) {
  unsigned i;
  for (i = 0; i < log.size; i++) {
    NONDET_CALL *call = (NONDET_CALL *)get_element_at(i, log);
    if (call == NULL)
      return;
    long long symbolic;
    memcpy(&symbolic, &call->value, sizeof(symbolic));
    long long concrete = klee_get_value_i64(symbolic);
    klee_assume(symbolic == concrete);
    memcpy(&call->value, &concrete, sizeof(concrete));
  }
}

/* Pinning costs a solver query per row, and the equalities pile up: with
 * 1,000 rows the violating path took 6 s, with 10,000 it never reached its
 * abort -- the target found, KLEE's timer expiring mid-log. Past this size the
 * log is not written: the violating state's .ktest, which KLEE solves in one
 * query when the state ends, carries the same vector in the same order, and
 * the frontend reads it whenever klee_log.csv is missing. */
#define MAP2CHECK_KLEE_LOG_MAX_ROWS 1024U

void nondet_generate_aux_witness_files() {
  MAP2CHECK_CONTAINER log = map2check_nondet_get_log();
  if (log.size > MAP2CHECK_KLEE_LOG_MAX_ROWS)
    return;
  concretize_nondet_log(log);
  nondet_log_to_file(log);
}


/* A failed assumption ends the path as a normal exit. klee_assume(0) on a
 * path where the condition is already false is a KLEE error ("invalid
 * klee_assume call (provably false)", a user.err), and the frontend must read
 * every KLEE error as a dropped path -- which made every program with an
 * assume_abort_if_not unprovable.
 *
 * klee_silent_exit avoided the error but wrote no test either, so a program
 * whose every path meets a failed assumption (most of XCSP) got an empty
 * Cover-Branches suite. _exit ends the path normally, so KLEE writes its
 * .ktest, and it skips the exit handlers: a path the program declared
 * infeasible reports no leak. */
extern void klee_make_symbolic(void *addr, size_t nbytes, const char *name);

/* The path's test still goes after every unpruned one in a Cover-Branches
 * suite (readKtestVectors), so the one-byte "map2check_pruned" object marks
 * it. The reader strips the marker; it is never an input. */
void nondet_assume(int expr) {
  if (!expr) {
    char pruned;
    klee_make_symbolic(&pruned, sizeof(pruned), "map2check_pruned");
    _exit(0);
  }
}

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
MAP2CHECK_NON_DET_GENERATOR(float)
MAP2CHECK_NON_DET_GENERATOR(longlong)
MAP2CHECK_NON_DET_GENERATOR(ulonglong)
MAP2CHECK_NON_DET_GENERATOR(u8)
MAP2CHECK_NON_DET_GENERATOR(u16)
MAP2CHECK_NON_DET_GENERATOR(u32)

/* A fresh object, copied in, rather than klee_make_symbolic(destination):
 * KLEE accepts only the exact start and size of an allocation, and the
 * benchmarks pass a field or a struct inside a larger one
 * (`__VERIFIER_nondet_memory(dest, sizeof(tdr_t))`). One object of `size`
 * bytes, not `size` objects of one: the reader expands it into one uchar
 * input per byte (decodeKtestInputs), as the validators' harness reads it. */
void map2check_non_det_memory(void *destination, size_t size) {
  if (size == 0)
    return;
  unsigned char *bytes = malloc(size);
  if (bytes == NULL)
    return;
  klee_make_symbolic(bytes, size, "non_det_memory");
  memcpy(destination, bytes, size);
  size_t i = 0;
  for (; i < size; i++) {
    map2check_nondet_memory_byte(0, 0, bytes[i], "__VERIFIER_nondet_memory");
  }
  free(bytes);
}
