#include <stdlib.h>

int *a, *b;

int main ()
{
    a = malloc (12 * sizeof(*a));
    b = malloc (12 * sizeof(*b));
    
    free(a);
    free(b); 
}
