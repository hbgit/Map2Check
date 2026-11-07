// Four reads inside a loop. A suite that recovers the values but drops one, or
// reorders them, cannot reach the error -- so this checks the ordering
// contract at a length where an off-by-one is visible.
extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);

int main(void) {
  int sum = 0;
  for (int i = 0; i < 4; i++) {
    sum += __VERIFIER_nondet_int();
  }
  if (sum == 10) {
    reach_error();
  }
  return 0;
}
