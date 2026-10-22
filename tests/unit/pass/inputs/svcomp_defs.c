/* SV-COMP style: the program defines library exports itself. */
#include <stdio.h>
#include <stdlib.h>
void reach_error(void) { abort(); }
void __VERIFIER_error(void) { abort(); }
void __VERIFIER_assert(int cond) {
  if (!cond) {
    reach_error();
  }
}
int __VERIFIER_nondet_int(void) { return 0; }
void __VERIFIER_assume(int cond) {
  if (!cond) abort();
}
int get_current_step(void) { return 42; }
int main(void) {
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x != 7);
  if (x == 123456) {
    printf("from the input %d\n", get_current_step());
  }
  __VERIFIER_assert(x != 99);
  if (x == 98) __VERIFIER_error();
  return 0;
}
