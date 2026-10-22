/* The definitions come after main, so a pass meets main's calls first. */
extern void abort(void);
int __VERIFIER_nondet_int(void);
void __VERIFIER_assert(int cond);
int main(void) {
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assert(x != 99);
  return 0;
}
int __VERIFIER_nondet_int(void) { return 0; }
void __VERIFIER_assert(int cond) {
  if (!cond) abort();
}
