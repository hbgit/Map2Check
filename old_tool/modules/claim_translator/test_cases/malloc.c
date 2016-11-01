#include <stdio.h>
#include <stdlib.h>

int func(){
	
	int r=12;
	
	double *x = malloc(r * sizeof(double));
	
	x[r] = 0.002;
	
	return 12;
	
}

main(){
    int a = func()/func();
}
