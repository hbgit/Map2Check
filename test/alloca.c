#include <stdio.h>

struct coiso {
    int var1;
    char var2[5];
    double teste;
};

int main() {
    int array[10];
    struct coiso teste[10];

    int simple;

    printf("Sizeof coiso %d\n", sizeof(struct coiso));
    return 0;
}
