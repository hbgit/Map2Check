extern void __VERIFIER_error() __attribute__ ((__noreturn__));
void __VERIFIER_assert(int cond) {
  if (!(cond)) {
    ERROR: __VERIFIER_error();
  }
  return;
}


int main() {

  int x=__VERIFIER_nondet_int();
  int y=__VERIFIER_nondet_int();
  
  int z=0;

  while(x>0) {
      x--;
      if(x==y) z=1;
  }

  __VERIFIER_assert(z==0);
}

