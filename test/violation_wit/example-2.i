extern void __VERIFIER_error(void);
extern int __VERIFIER_nondet_int(void);
int main() {
  unsigned int x = 1;
  if (__VERIFIER_nondet_int()) {
    x++;
  }
  if (__VERIFIER_nondet_int()) {
    x += __VERIFIER_nondet_int();
  }
  if (x == 42) __VERIFIER_error();
}
