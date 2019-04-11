int main() {

  int x=__VERIFIER_nondet_int();
  //__ESBMC_assume(x>0);
  int y=__VERIFIER_nondet_int();
  //__ESBMC_assume(y>=0);
  //__ESBMC_assume(y<1);
  int z=0;

  while(x>0) {
      x--;
      if(x==y) z=1; // assert(0);
  }
  __VERIFIER_assert(z==0);
}

