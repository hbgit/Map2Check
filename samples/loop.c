#include <stdio.h>

int main(int argc, char	**argv){
	int i, j,t = 0;
	for(i=0; i<10; i++){
		for(j=0; j<10; j++){
			if((i+j) % 7 == 0)
				break;
			else
				t++;
		}
	}
	printf("%d\n",	t);
	return	0;
}
