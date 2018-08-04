#include "../header/NonDetGenerator.h"
#include <stdlib.h>

/* Logic used for cases generation:
   1 - main function of original program is changed to _map2check_main
   2 - Fuzzer is used as a circular list
 */

#include <stdint.h>

extern int __map2check_main__();

const uint8_t* map2check_fuzzer_data;
size_t map2check_fuzzer_size;

uint8_t get_next_input_from_fuzzer() {
  static int i = 0;

  if(i < map2check_fuzzer_size) {
    return map2check_fuzzer_data[i++];
  }

  i = 0;
  return map2check_fuzzer_data[i];
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  map2check_fuzzer_data = Data;
  map2check_fuzzer_size = Size;
  return __map2check_main__();
}

int map2check_non_det_int() {
  return (int) get_next_input_from_fuzzer();
}

unsigned int map2check_non_det_uint() {
  return (unsigned int) get_next_input_from_fuzzer();
}

char map2check_non_det_char() {
  return (char) get_next_input_from_fuzzer();
}

unsigned short map2check_non_det_ushort() {
  return (short) get_next_input_from_fuzzer();
}

void* map2check_non_det_pointer() {
  // Ignore warnings from this, it is desired to be undefined behaviour
  return (void*) get_next_input_from_fuzzer();
}

long map2check_non_det_long() {
  return (long) get_next_input_from_fuzzer();
}

