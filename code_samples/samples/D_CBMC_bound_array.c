//./map2check-fortes.py -a code_samples/samples/D_CBMC_bound_array.c
//BUG no malloc
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char BYTE;

int a[5], b[6];


void return_foo(int c){
    BYTE  b1, b2;
    int i;
    for(i=0; i<5; i++)
        a[i] = i;

    b1 = '&';
}

#include <stdlib.h>

int main( ){
    unsigned int ka;
	int i, j, temp;
	short unsigned usa=65535;

	return_foo(3);
	int * p = malloc(10);
	for(i=0; i<5; i++){
		a[i]= a[i+1]+ i;		
		temp = a[i]*(i+1);
		for(j=0; j<temp; j++){
			b[j]= b[i-1]+(temp*2);
		}
	}
	//free(p);
	if(i > 0){
		exit(0);
		abort();
	}else{
	    double df = 0.0;
	    ka = 1234567;
	}

	float te = 1.5;
	return 0;
}

