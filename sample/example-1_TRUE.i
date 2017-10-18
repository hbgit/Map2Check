extern void __VERIFIER_error(void);
extern int __VERIFIER_nondet_int(void);

int main() {
  unsigned int x = 1;
  while(x > 10) {
    x = x + 2;
  }
  if (x >= 5) __VERIFIER_error();
  return 0;
}
