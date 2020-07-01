extern void __VERIFIER_error() __attribute__ ((__noreturn__));

void __VERIFIER_assert(int cond) {
  if (!(cond)) {
    ERROR: __VERIFIER_error();
  }
  return;
}

// Multi-byte data types

int main ()
{
    double sizes[] = { 10.3, 13.4, 11.2, 19.4 };
    double* p = sizes;
    
    ++p;                   // Advance p by sizeof(double)
    __VERIFIER_assert(*p == 13.4);    // The double at memory beginning at address p has value 13.4
}