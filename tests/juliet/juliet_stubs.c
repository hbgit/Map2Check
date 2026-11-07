/*
 * map2check_stubs.c — KLEE-friendly replacement for Juliet testcasesupport/io.c
 *
 * The stock io.c routes output through printf/wprintf and seeds randomness with
 * srand(time(NULL)) / rand(), which KLEE models as external calls and therefore
 * leaves every safe (good) program in a "partially completed path" state, making
 * Map2Check report UNKNOWN instead of TRUE.
 *
 * These stubs neutralise that I/O so KLEE can exhaust safe programs:
 *   - print*Line are no-ops (the bug, when present, happens *before* the sink);
 *   - globalReturnsTrueOrFalse() becomes nondeterministic via __VERIFIER_nondet_int
 *     (replaced by Map2Check's NonDetPass and made symbolic by the KLEE backend),
 *     so both branches are explored;
 *   - rand()/srand() are modelled as nondeterministic over [0, RAND_MAX]
 *     (see the long note above their definition — this is the one stub that
 *     changes what the verifier may conclude);
 *   - decodeHexChars/decodeHexWChars return 0 (hex-input variants are a small
 *     minority of the scope and are out of the first baseline).
 *
 * KNOWN BIAS, recorded so it is not mistaken for a result:
 *   - decodeHexChars returning 0 means "nothing decoded", which can turn a real
 *     bug into a trivially safe program. Those variants must be EXCLUDED from
 *     the scope rather than counted as easy true negatives.
 *   - printLine() in the stock io.c does `if (line != NULL) printf("%s\n", ...)`,
 *     i.e. it READS the buffer. As a no-op it stops reading, which can mask an
 *     out-of-bounds READ whose only sink is the print. 1924 CWE121 testcases
 *     call printLine(data); zero do in CWE190/191/369, so this does not affect
 *     the overflow scope — but it must be revisited before buffer-overflow CWEs
 *     enter the baseline.
 *
 * Link this (instead of io.c) into the Juliet bitcode passed to map2check.
 */
#include "std_testcase.h"
#include <stddef.h>
#include <stdlib.h>

#define NODISCARD(x) (void)(x)

void printLine(const char *line) { NODISCARD(line); }
void printWLine(const wchar_t *line) { NODISCARD(line); }
void printIntLine(int intNumber) { NODISCARD(intNumber); }
void printShortLine(short shortNumber) { NODISCARD(shortNumber); }
void printFloatLine(float floatNumber) { NODISCARD(floatNumber); }
void printLongLine(long longNumber) { NODISCARD(longNumber); }
void printLongLongLine(int64_t longLongIntNumber) { NODISCARD(longLongIntNumber); }
void printSizeTLine(size_t sizeTNumber) { NODISCARD(sizeTNumber); }
void printHexCharLine(char charHex) { NODISCARD(charHex); }
void printWcharLine(wchar_t wideChar) { NODISCARD(wideChar); }
void printUnsignedLine(unsigned unsignedNumber) { NODISCARD(unsignedNumber); }
void printHexUnsignedCharLine(unsigned char c) { NODISCARD(c); }
void printDoubleLine(double doubleNumber) { NODISCARD(doubleNumber); }
void printStructLine(const twoIntsStruct *s) { NODISCARD(s); }
void printBytesLine(const unsigned char *bytes, size_t numBytes) {
  NODISCARD(bytes);
  NODISCARD(numBytes);
}

size_t decodeHexChars(unsigned char *bytes, size_t numBytes, const char *hex) {
  NODISCARD(bytes); NODISCARD(numBytes); NODISCARD(hex);
  return 0;
}
size_t decodeHexWChars(unsigned char *bytes, size_t numBytes, const wchar_t *hex) {
  NODISCARD(bytes); NODISCARD(numBytes); NODISCARD(hex);
  return 0;
}

/*
 * rand()/srand() — modelled as nondeterministic, NOT neutralised.
 *
 * This is the one stub that changes what the verifier is allowed to conclude,
 * so the reasoning matters.
 *
 * Juliet feeds its "random" inputs through std_testcase.h:
 *     #define URAND31() (((unsigned)rand()<<30) ^ ((unsigned)rand()<<15) ^ rand())
 *     #define RAND32()  ((int)(rand() & 1 ? URAND31() : -URAND31() - 1))
 * Those are macros, expanded inside each testcase, so they cannot be overridden
 * from here — the only interception point is rand() itself.
 *
 * Left unstubbed, rand() is an unresolved external: KLEE calls the real libc
 * function and gets back ONE concrete value. The whole program then has a
 * single feasible path, and Map2Check answers VERIFICATION SUCCEEDED — not
 * UNKNOWN, but a confident wrong TRUE. Measured on the v3 baseline: all 6
 * CWE190 int_rand_add false negatives came from exactly this. (int_rand_square
 * escaped only by luck: data*data overflows for |data| > 46340, so almost any
 * concrete draw trips it. int_rand_add needs data == INT_MAX exactly.)
 *
 * Modelling an unknown external as nondeterministic is a SOUND
 * over-approximation — it admits every behaviour the concrete function could
 * have, plus possibly more — and is the standard treatment in SV-COMP. The
 * concrete-value alternative is the unsound one.
 *
 * "Plus possibly more" is where a naive stub creates FALSE POSITIVES, so the
 * range is constrained rather than left open: rand() is specified to return a
 * value in [0, RAND_MAX], and code like URAND31() above depends on that (it
 * assumes each draw contributes only low bits). Returning an unconstrained int
 * would hand the solver negative draws that real rand() cannot produce.
 *
 * The mask is exact only when RAND_MAX+1 is a power of two, which the static
 * assert enforces — if some libc ever breaks that, the build fails loudly
 * instead of silently biasing the input space. A masked nondet value is
 * preferred over __VERIFIER_assume here because it constrains by construction
 * and costs the solver no extra path splitting.
 */
_Static_assert((RAND_MAX & ((unsigned)RAND_MAX + 1u)) == 0,
               "RAND_MAX+1 is not a power of two, so `& RAND_MAX` no longer "
               "yields exactly [0, RAND_MAX]; use __VERIFIER_assume instead");

int rand(void) {
  extern int __VERIFIER_nondet_int(void);
  return __VERIFIER_nondet_int() & RAND_MAX;
}

/* Seeding is meaningless once rand() is symbolic, and the stock call is
 * srand(time(NULL)) — time() being another unresolved external that pins KLEE
 * to a concrete value. Dropping it removes the external without affecting the
 * input space, which rand() above now owns entirely. */
void srand(unsigned seed) { NODISCARD(seed); }

int globalReturnsTrue(void) { return 1; }
int globalReturnsFalse(void) { return 0; }
int globalReturnsTrueOrFalse(void) {
  int v;
  extern int __VERIFIER_nondet_int(void);
  v = __VERIFIER_nondet_int();
  return v & 1;
}

const int GLOBAL_CONST_TRUE = 1;
const int GLOBAL_CONST_FALSE = 0;
const int GLOBAL_CONST_FIVE = 5;

int globalTrue = 1;
int globalFalse = 0;
int globalFive = 5;

void good1(void) {}
void good2(void) {}
void good3(void) {}
void good4(void) {}
void good5(void) {}
void good6(void) {}
void good7(void) {}
void good8(void) {}
void good9(void) {}

void bad1(void) {}
void bad2(void) {}
void bad3(void) {}
void bad4(void) {}
void bad5(void) {}
void bad6(void) {}
void bad7(void) {}
void bad8(void) {}
void bad9(void) {}

int globalArgc = 0;
char **globalArgv = NULL;
