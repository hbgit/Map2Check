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
 *   - decodeHexChars/decodeHexWChars return 0 (hex-input variants are a small
 *     minority of the scope and are out of the first baseline).
 *
 * Link this (instead of io.c) into the Juliet bitcode passed to map2check.
 */
#include "std_testcase.h"
#include <stddef.h>

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
