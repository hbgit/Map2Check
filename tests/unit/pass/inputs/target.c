extern int __VERIFIER_nondet_int(void);
extern void reach_error(void);
int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 42) {
    reach_error();
  }
  return 0;
}
