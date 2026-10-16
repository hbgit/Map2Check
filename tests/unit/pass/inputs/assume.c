extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int cond);
extern void __VERIFIER_assert(int cond);
static int step(int x) {
  __VERIFIER_assume(x > 0);
  return x - 1;
}
int main(void) {
  int x = __VERIFIER_nondet_int();
  int y = step(x);
  __VERIFIER_assert(y >= 0);
  return 0;
}
