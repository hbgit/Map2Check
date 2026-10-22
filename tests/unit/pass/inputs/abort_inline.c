extern void abort(void);
extern int __VERIFIER_nondet_int(void);
void reach_error(void) {}
int main(void) {
  int i2 = __VERIFIER_nondet_int();
  if (!(i2)) {
    abort();
  }
  if (i2 == 7) {
    reach_error();
    abort();
  }
  return 0;
}
