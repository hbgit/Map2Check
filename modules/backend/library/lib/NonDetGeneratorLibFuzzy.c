#include "../header/NonDetGenerator.h"
#include "../header/NonDetLog.h"
#include <stdlib.h>

/* Logic used for cases generation:
   1 - main function of original program is changed to _map2check_main
   2 - Fuzzer is used as a circular list
 */

#include <stdint.h>

extern int __map2check_main__();
#include <pthread.h>

#include "../header/Map2CheckFunctions.h"
void fuzzer_giveup_current_case() { map2check_destroy(); }

void *fuzzer_execution_function(void *args) {
  __map2check_main__();
  return NULL;
}

pthread_t fuzzer_execution;

void nondet_init() { nondet_log_init(); }
void nondet_destroy() { nondet_log_destroy(); }
void nondet_cancel() { pthread_cancel(fuzzer_execution); }
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
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &prevType);
  pthread_create(&fuzzer_execution, NULL, fuzzer_execution_function, NULL);
  pthread_join(fuzzer_execution, NULL);
  return 0;
}

#define MAP2CHECK_NON_DET_GENERATOR(type)                                      \
  type map2check_non_det_##type() { return (type)get_next_input_from_fuzzer(); }

MAP2CHECK_NON_DET_GENERATOR(int)
MAP2CHECK_NON_DET_GENERATOR(char)
MAP2CHECK_NON_DET_GENERATOR(pointer)
MAP2CHECK_NON_DET_GENERATOR(ushort)
MAP2CHECK_NON_DET_GENERATOR(long)
MAP2CHECK_NON_DET_GENERATOR(unsigned)
MAP2CHECK_NON_DET_GENERATOR(ulong)
MAP2CHECK_NON_DET_GENERATOR(bool)
MAP2CHECK_NON_DET_GENERATOR(uchar)
MAP2CHECK_NON_DET_GENERATOR(size_t)
MAP2CHECK_NON_DET_GENERATOR(loff_t)
MAP2CHECK_NON_DET_GENERATOR(sector_t)
MAP2CHECK_NON_DET_GENERATOR(uint)

char *map2check_non_det_pchar() {
  unsigned length = map2check_non_det_unsigned();
  if (length == 0)
    return NULL;
  char string[length];
  unsigned i = 0;
  for (i = 0; i < (length - 1); i++) {
    string[i] = map2check_non_det_char();
  }
  string[i] = '\0';
  return string;
}
