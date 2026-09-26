/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include "../header/NonDetGenerator.h"
#include "../header/NonDetLog.h"

#include <stdlib.h>
#include <stdint.h>
#include <setjmp.h>

/* Logic used for cases generation:
   1 - main function of original program is changed to _map2check_main
   2 - AFL++ persistent mode feeds one test case per __AFL_LOOP iteration
 */

extern int __map2check_main__(int argc, char **argv);

#include "../header/Map2CheckFunctions.h"

void nondet_init() { nondet_log_init(); }

void nondet_destroy() { nondet_log_destroy(); }

static jmp_buf map2check_reject_env;

void nondet_cancel() { longjmp(map2check_reject_env, 1); }

void nondet_assume(int expr) {
  if (!expr) {
    nondet_cancel();
  }
}

void nondet_generate_aux_witness_files() {
  nondet_log_to_file(map2check_nondet_get_log());
}

const uint8_t *map2check_afl_data;

size_t map2check_afl_size;

uint8_t get_next_input_from_afl() {
  static int i = 0;
  if (i < map2check_afl_size) {
    return map2check_afl_data[i++];
  }

  i = 0;
  return map2check_afl_data[i];
}

/* Fills `out` with `size` bytes from the AFL buffer, in target order.
 *
 * Same width contract as NonDetGeneratorKlee.c: sizeof(type) bytes per value,
 * so a vector means the same thing to both engines and seeding stays sound. */
static void get_bytes_from_afl(void *out, size_t size) {
  unsigned char *destination = (unsigned char *)out;
  size_t i = 0;
  for (; i < size; i++) {
    destination[i] = get_next_input_from_afl();
  }
}

#define MAP2CHECK_NON_DET_GENERATOR(type)                                      \
  type map2check_non_det_##type() {                                            \
    type value;                                                                \
    get_bytes_from_afl(&value, sizeof(value));                                 \
    return value;                                                              \
  }

MAP2CHECK_NON_DET_GENERATOR(char)
MAP2CHECK_NON_DET_GENERATOR(pointer)
MAP2CHECK_NON_DET_GENERATOR(ushort)
MAP2CHECK_NON_DET_GENERATOR(short)
MAP2CHECK_NON_DET_GENERATOR(long)
MAP2CHECK_NON_DET_GENERATOR(ulong)
MAP2CHECK_NON_DET_GENERATOR(bool)
MAP2CHECK_NON_DET_GENERATOR(uchar)
MAP2CHECK_NON_DET_GENERATOR(size_t)
#ifndef __INTELLISENSE__
MAP2CHECK_NON_DET_GENERATOR(loff_t)
#endif
MAP2CHECK_NON_DET_GENERATOR(sector_t)
MAP2CHECK_NON_DET_GENERATOR(double)
MAP2CHECK_NON_DET_GENERATOR(int)
MAP2CHECK_NON_DET_GENERATOR(uint)
MAP2CHECK_NON_DET_GENERATOR(unsigned)

#define MAP2CHECK_MAX_FUZZED_STRING 4096

char *map2check_non_det_pchar() {
  unsigned length = map2check_non_det_unsigned();
  if (length == 0)
    return NULL;
  if (length > MAP2CHECK_MAX_FUZZED_STRING)
    length = MAP2CHECK_MAX_FUZZED_STRING;
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

/* AFL++ persistent-mode trampoline.
 *
 * __AFL_FUZZ_INIT registers the shared-memory test case. __AFL_LOOP runs the
 * body once per input under afl-fuzz; run standalone (replaying a saved crash
 * file as argv[1]) it runs exactly once with that file as input.
 *
 * A failed nondet_assume longjmps back here and skips the input — the
 * persistent-mode equivalent of the pthread_exit the previous fuzzer generator
 * used (a rejected input, not a crash). */
__AFL_FUZZ_INIT();

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;
  while (__AFL_LOOP(10000)) {
    if (setjmp(map2check_reject_env) == 0) {
      map2check_afl_data = __AFL_FUZZ_TESTCASE_BUF;
      map2check_afl_size = __AFL_FUZZ_TESTCASE_LEN;
      __map2check_main__(0, NULL);
    }
    /* else: input rejected by nondet_assume; continue to the next iteration */
  }
  return 0;
}
