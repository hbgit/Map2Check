extern int __VERIFIER_nondet_int(void);
int helper(int x) { return x + 1; }
int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x > 3) {
    x = helper(x);
  }
  return x;
}
