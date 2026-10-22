#include <stdio.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int cond);
/* noinline: at -O2 the rejection still unwinds through a C frame. */
__attribute__((noinline)) static int step(int x) {
  __VERIFIER_assume(x > 0);
  return x - 1;
}
int main(void) {
  int x = __VERIFIER_nondet_int();
  int y = step(x);
  printf("accepted %d\n", y);
  return 0;
}
