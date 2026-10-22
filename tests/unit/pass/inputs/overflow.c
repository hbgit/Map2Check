extern int __VERIFIER_nondet_int(void);
extern long __VERIFIER_nondet_long(void);
int main(void) {
  int a = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();
  int s = a + b;
  int d = a - b;
  int m = a * b;
  int q = b != 0 ? a / b : 0;
  int r = b != 0 ? a % b : 0;
  long w = __VERIFIER_nondet_long() + 1L;
  unsigned u = (unsigned)a + 1u;
  return (s ^ d ^ m ^ q ^ r ^ (int)w ^ (int)u) & 1;
}
