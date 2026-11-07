// A char-typed read. NonDetLog.c formats the logged value according to the
// nondet type, so a type other than int exercises a distinct branch of the
// serializer -- the same kind of branch where the double type tag was wrong.
extern char __VERIFIER_nondet_char(void);
extern void reach_error(void);

int main(void) {
  char c = __VERIFIER_nondet_char();
  if (c == 'A') {
    reach_error();
  }
  return 0;
}
