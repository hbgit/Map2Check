#include <stdio.h>
#include <stdlib.h>

int main()
{
    int *p = malloc(12);
    free(p);
    return 0;
}
