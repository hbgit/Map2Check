extern void __assert_fail(const char *, const char *, unsigned int,
                          const char *) __attribute__((__noreturn__));
/* As in sv-benchmarks: reach_error is defined, so that the program links
   natively for the AFL++ phase, and not only as bitcode for KLEE. */
void reach_error(void) { __assert_fail("0", "cover_error.c", 5, "reach_error"); }
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 42) {
    reach_error();
  }
  return 0;
}
