#include <stdlib.h>

struct Node {
	int value;
	struct Node* next;
};


main(){
	// Allocate the pointers
	struct Node* x;
	struct Node* y;
	struct Node* z;

	// Allocate the pointees
	x = malloc(sizeof(Node));
	y = malloc(sizeof(Node));
	z = malloc(sizeof(Node));

	// Put the numbers in the pointees
	x->value = 1;
	y->value = 2;
	z->value = 3;

	// Put the pointers in the pointees
	x->next = y;
	y->next = z;
	z->next = x;
}
