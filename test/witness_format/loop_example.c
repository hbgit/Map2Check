#include <stdlib.h>

extern void __VERIFIER_error(void);

int main() {
    int a = 10;
    int c = 1 || (a = 0);
    int *p = NULL;
    if (a) {
        while(c < a){
            p = &a;
            c++;
        }
        __VERIFIER_error();
        return 1;
    }
    return 0;
}    
