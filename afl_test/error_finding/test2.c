#include "../lib_gen/lib_gen.h"
#include <stdlib.h>
// Aborts if input 007
int main() {
  int test[100];
  int i = 0;
  for(; i < 100; i++) {
    test[i] = __VERIFIER_nondet_int();
  }

  if(test[99] == 65) {
    abort();
  }
  return 0;
}
