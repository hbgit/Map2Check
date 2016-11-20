#include <stdio.h>
#include <stdlib.h>

main(){
    int *p;
	int *A = malloc(10);
	int *B = malloc(10); //FORGETTEN MEMORY in the line 11
	
	B = A;
    
    B = p;
	
	free(A);
	free(B); //INVALID
}
