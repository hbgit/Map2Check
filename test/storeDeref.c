#include <stdlib.h>

struct X {
    void* a;
    void* b;
};

int main() {
    struct X VarOk;
    VarOk.a = (void*) malloc(sizeof(int));
    VarOk.b = VarOk.a;

    int* pointer = ((int*)VarOk.b);
    int coiso = *pointer;
    VarOk.a = NULL;
    free(VarOk.b);

    return 0;
}
