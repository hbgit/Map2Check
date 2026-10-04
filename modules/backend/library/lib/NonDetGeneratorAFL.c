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

/* Read position in the current test case. File scope, not function-static,
 * so main() can rewind it for every __AFL_LOOP iteration: persistent mode
 * runs many inputs in one process, and a position carried over from the
 * previous input makes the same input read different values on every run.
 * That nondeterminism is what defeats CmpLog's input-to-state matching and
 * drags afl-fuzz's stability down. */
static size_t map2check_afl_index = 0;

/* Past the end of the test case every read is zero. It used to start over
 * from the first byte, so `while (__VERIFIER_nondet_int())` fed by the
 * placeholder seed "A" never ended: afl-fuzz's dry run timed out on it and the
 * fuzzer aborted before its first execution. Zero ends such a loop, and it is
 * what the witness replay logs, so the suite built from that log stays exact.
 * (It also read data[0] of an empty test case.) */
uint8_t get_next_input_from_afl() {
  if (map2check_afl_index < map2check_afl_size) {
    return map2check_afl_data[map2check_afl_index++];
  }
  return 0;
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

/* The persistent-mode macros, as afl-cc 4.40c defines them (src/afl-cc.c).
 *
 * afl-cc injects these with -D only when IT compiles C source. This file is
 * compiled to bitcode by plain clang (the library build must not depend on
 * AFL++, and a KLEE-only build never sees afl-cc), and afl-clang-fast later
 * receives the linked -result.bc, which is never preprocessed again. So the
 * expansions have to be in the bitcode already -- including the
 * ##SIG_AFL_PERSISTENT## marker afl-fuzz looks for in the binary to switch to
 * persistent mode. The __afl_* symbols resolve from afl-compiler-rt at that
 * final afl-clang-fast link. Keep in sync with the pinned AFL++ tag. */
#ifndef __AFL_FUZZ_TESTCASE_LEN
#include <unistd.h>

#define __AFL_FUZZ_INIT()                                                      \
  int __afl_sharedmem_fuzzing = 1;                                             \
  extern __attribute__((visibility("default"))) unsigned int *__afl_fuzz_len;  \
  extern __attribute__((visibility("default"))) unsigned char *__afl_fuzz_ptr; \
  unsigned char __afl_fuzz_alt[1048576];                                       \
  unsigned char *__afl_fuzz_alt_ptr = __afl_fuzz_alt

#define __AFL_FUZZ_TESTCASE_BUF (__afl_fuzz_ptr ? __afl_fuzz_ptr : __afl_fuzz_alt_ptr)

#define __AFL_FUZZ_TESTCASE_LEN                                                \
  (__afl_fuzz_ptr ? *__afl_fuzz_len                                            \
   : (*__afl_fuzz_len = read(0, __afl_fuzz_alt_ptr, 1048576)) == 0xffffffff   \
       ? 0                                                                     \
       : *__afl_fuzz_len)

#define __AFL_LOOP(_A)                                                         \
  ({                                                                           \
    static volatile const char *_B __attribute__((used, unused));             \
    _B = (const char *)"##SIG_AFL_PERSISTENT##";                               \
    extern __attribute__((visibility("default"))) int __afl_connected;        \
    __attribute__((visibility("default"))) int _L(unsigned int) __asm__(      \
        "__afl_persistent_loop");                                              \
    _L(__afl_connected ? _A : 1);                                              \
  })
#endif

/* AFL++ persistent-mode trampoline.
 *
 * __AFL_FUZZ_INIT registers the shared-memory test case. __AFL_LOOP runs the
 * body once per input under afl-fuzz; run standalone (replaying a saved crash
 * file) it runs exactly once and reads that input from STDIN -- argv is
 * ignored, so the replay must redirect the file in, not pass it as argument.
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
      map2check_afl_index = 0;
      __map2check_main__(0, NULL);
    }
    /* else: input rejected by nondet_assume; continue to the next iteration */
  }
  return 0;
}
