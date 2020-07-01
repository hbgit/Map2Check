extern void __VERIFIER_error() __attribute__ ((__noreturn__));

void __VERIFIER_assert(int cond) {
  if (!(cond)) {
    ERROR: __VERIFIER_error();
  }
  return;
}

// Dereferencing and accessing a structure data member
typedef struct X { int i_; double d_; } X;

int main ()
{
    X x;
    X* p = &x;
    p->d_ = 3.14159;  // Dereference and access data member x.d_
    (*p).d_ *= -1;    // Another equivalent notation for accessing x.d_

    __VERIFIER_assert(p->d_ >= 2.0);  
}



