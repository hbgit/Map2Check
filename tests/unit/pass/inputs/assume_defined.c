#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);
void assume_abort_if_not(int cond) {
  if (!cond) abort();
}
void __VERIFIER_assume(int cond) {
  if (!cond) abort();
}
int main(void) {
  int x = __VERIFIER_nondet_int();
  assume_abort_if_not(x > 0);
  __VERIFIER_assume(x < 100);
  return 0;
}
