#include <klee/klee.h>
#include <assert.h>

#define __VERIFIER_assume KLEE_assume
#define __VERIFIER_assert KLEE_assert

void KLEE_assume(int condition) {
    // Guard the condition we assume so that if it is not
    // satisfiable we don't flag up an error. Instead we'll
    // just silently terminate this state.
    if (condition) {
        klee_assume(condition);
    } else {
        klee_silent_exit(0);
    }
}

#define KLEE_assert(cond) {if (!(cond)) klee_report_error(__FILE__, __LINE__ , "svcomp assert failed", "svcomp.assertfail"); }

void __VERIFIER_error()  {
    klee_report_error(__FILE__, __LINE__, "svcomp error", "svcomp.err");
}

#define __VERIFIER_nondet_int KLEE_nondet_int
// int KLEE_nondet_int() { int x; klee_make_symbolic(&x, sizeof(x), "x"); return x; }
int KLEE_nondet_int() { return klee_int("x"); }
#define __VERIFIER_nondet_uint KLEE_nondet_uint
unsigned int KLEE_nondet_uint() { unsigned int x; klee_make_symbolic(&x, sizeof(x), "x"); return x; }
#define __VERIFIER_nondet_bool KLEE_nondet_bool
// _Bool KLEE_nondet_bool() { _Bool x; klee_make_symbolic(&x, sizeof(x), "x"); return x; }
_Bool KLEE_nondet_bool() { return klee_range(0, 2, "x"); }
#define __VERIFIER_nondet_char KLEE_nondet_char
char KLEE_nondet_char() { char x; klee_make_symbolic(&x, sizeof(x), "x"); return x; }
#define __VERIFIER_nondet_uchar KLEE_nondet_uchar
unsigned char KLEE_nondet_uchar() { unsigned char x; klee_make_symbolic(&x, sizeof(x), "x"); return x; }
