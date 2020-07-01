extern void __VERIFIER_error() __attribute__ ((__noreturn__));

void __VERIFIER_assert(int cond) {
  if (!(cond)) {
    ERROR: __VERIFIER_error();
  }
  return;
}

extern int __VERIFIER_nondet_int(void);

int main ()
{
   int a = __VERIFIER_nondet_int();
   int b = __VERIFIER_nondet_int();

   int c = a + b;
}
