#include <stdio.h>
#include <stdlib.h>

main(){

	int *A = malloc(10);
	int *B = malloc(10); 
	
	int *C;
	
	*C = malloc(5);
	
	B = C; //Como esta sendo feito o mapeamento????
	
	free(A);
	free(B); 
}
