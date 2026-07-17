#include <stdlib.h>
#include <stdio.h>

int main() {
    int *ptr = (int *)malloc(sizeof(int)); 
    *ptr = 42;                             
    free(ptr); // Free the memory

    // Use the pointer after freeing the memory (use-after-free)
    // Undefined behavior, potential crash or exploit
    printf("%d\n", *ptr);                  
    return 0;
}
