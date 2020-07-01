// based on https://www.cs.utexas.edu/~shmat/courses/cs380s_fall09/blexim.txt

extern void __VERIFIER_error() __attribute__ ((__noreturn__));

void __VERIFIER_assert(int cond) {
  if (!(cond)) {
    ERROR: __VERIFIER_error();
  }
  return;
}

extern int __VERIFIER_nondet_int(void);
/*
The addition is causing an overflow in exactly the same way as the first
example, and so is the multiplication, although it may seem different.  In
both cases the result of the arithmetic is too great to fit in an integer,
so it is reduced as described above.  The subtraction is slightly
different, as it is causing an underflow rather than an overflow: an
attempt is made to store a value lower than the minimum value the integer
can hold, causing a wrap around.  In this way we are able to force an
addition to subtract, a multiplication to divide or a subtraction to add.
**/

int main ()
{
    int l, x;

    l = 0x40000000;    

    x = l + 0xc0000000;
    
    x = l * 0x4;
    
    x = l - 0xffffffff;
    
    return 0;
}
