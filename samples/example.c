extern void __VERIFIER_error() __attribute__((__noreturn__));
extern int __VERIFIER_nondet_int(void);
extern int __VERIFIER_nondet_double(void);

int bar(){
  int a;// = __VERIFIER_nondet_int(); 
  return a;
}

void foo() {
  int N = bar();
  int i =0;
  for(i = 0; i<= N; i++){
    if(i > 1){
      __VERIFIER_error();
    }

  }
}

int main(){
  foo();
  int b;// = __VERIFIER_nondet_int();
  return 0;
}