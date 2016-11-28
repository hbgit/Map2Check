#include <stdio.h>
#include <stdlib.h>

main(){

	int *A = malloc(10);
	int *B = malloc(10);
	
	//B = A;
    free(A);
    free(A);
}
