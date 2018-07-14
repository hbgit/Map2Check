#ifndef TEST_UTIL_
#define TEST_UTIL_
#include <stdio.h>
#include <stdlib.h>

void assert(int val, unsigned line) {
  if (!val) {
    printf("ERROR ON LINE %u\n", line);
    exit(-1);
  }
}
#endif
