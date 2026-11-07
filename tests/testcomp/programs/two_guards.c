// Two independent nondeterministic reads guard one error call. A suite that
// recovers both values but emits them in the wrong order never reaches
// reach_error, so this program tests the ordering contract and not only value
// recovery.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);

int main(void) {
  int a = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();
  if (a == 42) {
    if (b == 7) {
      reach_error();
    }
  }
  return 0;
}
