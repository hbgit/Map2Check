extern int __VERIFIER_nondet_int(void);

int main(void) {
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();
  if (x > 10) {
    y = y + 1;
  } else {
    y = y - 1;
  }
  if (y == 7) {
    return 1;
  }
  return 0;
}
