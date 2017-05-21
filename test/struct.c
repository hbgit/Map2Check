#include <stdio.h>

struct x {
    char var1;
    int var2[2];
};

int main() {
    struct x X;

    X.var2[0] = 3;
    printf("%d \n", sizeof(X.var2));

    void* okPointer = &X;
    int teste = ((struct x*)okPointer)->var2[0];
    //okPointer += sizeof(char);

    //int value = *(int*)okPointer;
    //printf("Valor: %d\n", value);
    return 0;

}
