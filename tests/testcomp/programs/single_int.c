// One nondeterministic read, one guard: the simplest suite that can exist.
// A failure here means the mechanism is broken, not that an edge case is.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 99) {
    reach_error();
  }
  return 0;
}
