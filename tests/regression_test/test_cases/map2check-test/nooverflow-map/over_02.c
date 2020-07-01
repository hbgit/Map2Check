// based on https://www.cs.utexas.edu/~shmat/courses/cs380s_fall09/blexim.txt

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
    int l;

    l = 0x7fffffff;

    // When it is incremented, the most significant bit
    // (indicating signedness) is set and the integer is interpreted as being
    // negative.
    __VERIFIER_assert(l + 1 >= 0);
}
