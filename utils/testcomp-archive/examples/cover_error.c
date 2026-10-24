extern void reach_error(void);
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 42) {
    reach_error();
  }
  return 0;
}
