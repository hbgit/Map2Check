#include <stdio.h>
#include <stdlib.h>

int k = 0;

main(){

	int *A = malloc(10);
	int *B = malloc(10); //FORGETTEN MEMORY in the line 11
	
	int C = 12;
	B = &C;
	
	free(A);
	free(B); //INVALID
}
