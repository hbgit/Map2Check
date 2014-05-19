#include <stdio.h>

int a[5], b[6];

void foo(){
    int i;
    for(i=0; i<5; i++)
        a[i] = i;
}

int main( ){
	int i, j, temp; 	
	for(i=0; i<5; i++){
		a[i]= a[i+1]+ i;		
		temp = a[i]*(i+1);
		for(j=0; j<temp; j++){
			b[j]= b[i-1]+(temp*2);
		}
	}
}

