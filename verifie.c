extern void __VERIFIER_error() __attribute__ ((__noreturn__));

#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);

main(){
	
	int a = 1;
	int *b = malloc(10);
	
	if(a){
		__VERIFIER_error();
	}else{
		a++;
		b = &a;
	}
	
	free(b);
}
