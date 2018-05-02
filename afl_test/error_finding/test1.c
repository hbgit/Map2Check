#include "../lib_gen/lib_gen.h"
#include <stdlib.h>
// Aborts if input 007
int main() {
  if(__VERIFIER_nondet_int() == 0) {
    if(__VERIFIER_nondet_int() == 0) {
      if(__VERIFIER_nondet_int() == 7) {
        abort();
      }
    }
  }  
  return 0;
}
