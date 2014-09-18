#include <stdio.h>

int a[5], b[6];

void return_foo(int c){
    int i;
    for(i=0; i<5; i++)
        a[i] = i;    
}

#include <stdlib.h>

int main( ){
	int i, j, temp; 
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
	}
	return 0;
}

