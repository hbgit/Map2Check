extern void __VERIFIER_error() __attribute__ ((__noreturn__));

void __VERIFIER_assert(int cond) {
  if (!(cond)) {
    ERROR: __VERIFIER_error();
  }
  return;
}

int main ()
{
    const char* p = "abc";

    __VERIFIER_assert(*p == 'a');
}
