#include <stdio.h>
#include <stdlib.h>

int main()
{
    int* a = (int*) calloc(3, sizeof(int));
    int* ref = a;

    *a = 5;
    a++;

    *a = 10;
    a++;

    *a = 15;
    a++;

    *a = 20;

    free(ref);



    return 0;
}
