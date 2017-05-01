#include <stdio.h>
#include <stdlib.h>

main(){

	int *A = malloc(10);
	int *B = malloc(10); //FORGETTEN MEMORY in the line 11
	
	if(A){
		B = A;
	}
	
	free(A);
	free(B); //INVALID
}
