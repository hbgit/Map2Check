#include <stdlib.h>

int *a;

int main ()
{
    a = malloc (12 * sizeof(*a));    
    free(a);
    free(a); // it was released
}
