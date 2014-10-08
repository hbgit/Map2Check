#include <stdio.h>
#include <stdlib.h>

extern void __VERIFIER_error(void);                                             
 
#define ERRORMESSAGE(text) fprintf( stderr, \
            "File [%s], line %d: %s\n" \
            __FILE__, __LINE__, text )

int main() {
    int r = __VERIFIER_nondet_int();
    int a,b;

    if (r) {
        a = 1;
        b = 10;
        while (b > 0) {
            b = b - 1;
        }
    } else {
        a = 2;
    }

#pragma STDC FENV_ACCESS ON

    if (a == 2) {
        ERRORMESSAGE("Error!!");
        __VERIFIER_error();
        return 1;
    }

    return 0;
}    
