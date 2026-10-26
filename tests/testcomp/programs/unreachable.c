// reach_error() is unreachable: the guard is unsatisfiable. No suite can cover
// it. This is the negative control -- if the harness ever reports this one as
// COVERED then the harness is lying, and every other PASS in this corpus is
// meaningless. A gate that cannot fail is not a gate.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x > 0 && x < 0) {
    reach_error();
  }
  return 0;
}
