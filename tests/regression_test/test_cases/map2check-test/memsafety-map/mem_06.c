extern void __VERIFIER_error() __attribute__ ((__noreturn__));

void __VERIFIER_assert(int cond) {
  if (!(cond)) {
    ERROR: __VERIFIER_error();
  }
  return;
}

int main ()
{
    int x = 2;
    int* p_x = &x;  // Put the address of the x variable into the pointer p_x
    *p_x = 4;       // Change the memory at the address in p_x to be 4
    __VERIFIER_assert(x == 4); // Check x is now 4    
}



