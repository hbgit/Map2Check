#include "../header/NonDetGenerator.h"

/* This mode returns consts */

#include <stdint.h>


int map2check_non_det_int() {
  return 0;
}

unsigned int map2check_non_det_uint() {
  return 0;
}

char map2check_non_det_char() {
  return 0;
}

unsigned short map2check_non_det_ushort() {
  return 0;
}

void* map2check_non_det_pointer() {
  // Ignore warnings from this, it is desired to be undefined behaviour
  return 0;
}

long map2check_non_det_long() {
  return 0;
}

