#include <stdio.h>

int addInt(int n, int m) {
    return n + m;
}

int main() {
    int (*functionPtr) (int,int);
    printf("Adress of function: %p", &main);
    functionPtr = &addInt;

    return 0;
}



