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
#include <pthread.h>
#include <signal.h>

/* Logic used for cases generation:
   1 - main function of original program is changed to _map2check_main
   2 - Fuzzer is used as a circular list
 */

extern int __map2check_main__(int argc, char **argv);

#include "../header/Map2CheckFunctions.h"

void *fuzzer_execution_function(void *args) {
  (void)args;
  __map2check_main__(0, NULL);
  return NULL;
}

pthread_t fuzzer_execution;

void nondet_init() { nondet_log_init(); }

void nondet_destroy() { nondet_log_destroy(); }

void nondet_cancel() { pthread_exit(NULL); }

void nondet_assume(int expr) {
  if (!expr) {
    nondet_cancel();
  }
}

void nondet_generate_aux_witness_files() {
  nondet_log_to_file(map2check_nondet_get_log());
}

const uint8_t *map2check_fuzzer_data;

size_t map2check_fuzzer_size;

uint8_t get_next_input_from_fuzzer() {
  static int i = 0;
  if (i < map2check_fuzzer_size) {
    return map2check_fuzzer_data[i++];
  }

  i = 0;
  return map2check_fuzzer_data[i];
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  map2check_fuzzer_data = Data;
  map2check_fuzzer_size = Size;
  int prevType;
  // int currentProccess = getpid();
  // printf("Creating %d\n", currentProccess);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &prevType);
  pthread_cleanup_push(map2check_destroy, NULL);
  pthread_create(&fuzzer_execution, NULL, fuzzer_execution_function, NULL);
  pthread_join(fuzzer_execution, NULL);
  pthread_cleanup_pop(0);
  // map2check_destroy();
  return 0;
}

/* Fills `out` with `size` bytes from the fuzzer's buffer, in target order.
 *
 * The generators below used to take ONE byte and cast it, whatever the type.
 * A `long` could therefore only ever be 0..255; so could a `short`, a
 * `size_t`, a pointer -- and a `double` could only be an integral value
 * between 0.0 and 255.0. Nothing negative was reachable at all, because an
 * unsigned byte cast to a signed type stays non-negative.
 *
 * Beyond the obvious loss of reach, this is what made seeding impossible: the
 * byte layout IS the exchange format between the two engines, and a KLEE
 * vector holding short x = 4242 cannot be written into a slot one byte wide.
 * Consuming sizeof(type) puts the fuzzer on the same layout KLEE already uses
 * -- NonDetGeneratorKlee.c passes sizeof(non_det) to klee_make_symbolic -- so
 * a vector means the same thing to both. */
static void get_bytes_from_fuzzer(void *out, size_t size) {
  unsigned char *destination = (unsigned char *)out;
  size_t i = 0;
  for (; i < size; i++) {
    destination[i] = get_next_input_from_fuzzer();
  }
}

#define MAP2CHECK_NON_DET_GENERATOR(type)                                      \
  type map2check_non_det_##type() {                                            \
    type value;                                                                \
    get_bytes_from_fuzzer(&value, sizeof(value));                              \
    return value;                                                              \
  }

MAP2CHECK_NON_DET_GENERATOR(char)
MAP2CHECK_NON_DET_GENERATOR(pointer)
MAP2CHECK_NON_DET_GENERATOR(ushort)
MAP2CHECK_NON_DET_GENERATOR(short)
MAP2CHECK_NON_DET_GENERATOR(long)
// MAP2CHECK_NON_DET_GENERATOR(unsigned)
MAP2CHECK_NON_DET_GENERATOR(ulong)
MAP2CHECK_NON_DET_GENERATOR(bool)
MAP2CHECK_NON_DET_GENERATOR(uchar)
MAP2CHECK_NON_DET_GENERATOR(size_t)
#ifndef __INTELLISENSE__
MAP2CHECK_NON_DET_GENERATOR(loff_t)
#endif
MAP2CHECK_NON_DET_GENERATOR(sector_t)
MAP2CHECK_NON_DET_GENERATOR(double)
// MAP2CHECK_NON_DET_GENERATOR(uint)

/* Was reading EIGHT bytes and truncating to int, so half of every integer's
 * worth of fuzzer entropy was consumed and thrown away -- and, worse for
 * seeding, the layout did not match what KLEE writes for the same read. */
MAP2CHECK_NON_DET_GENERATOR(int)

MAP2CHECK_NON_DET_GENERATOR(uint)
MAP2CHECK_NON_DET_GENERATOR(unsigned)

/* Upper bound on a fuzzer-chosen string length.
 *
 * The length comes from a full-width unsigned, so before this the malloc below
 * could be asked for four billion bytes on a whim. Any string long enough to
 * matter for a benchmark fits well inside this. */
#define MAP2CHECK_MAX_FUZZED_STRING 4096

char *map2check_non_det_pchar() {
  unsigned length = map2check_non_det_unsigned();
  if (length == 0)
    return NULL;
  if (length > MAP2CHECK_MAX_FUZZED_STRING)
    length = MAP2CHECK_MAX_FUZZED_STRING;
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
