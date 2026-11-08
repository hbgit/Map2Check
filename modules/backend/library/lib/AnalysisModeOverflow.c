/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include <limits.h>
#include <stdio.h>

#include "../header/AnalysisMode.h"
#include "../header/Map2CheckFunctions.h"
#include "../header/PropertyGenerator.h"
// TODO(rafa.sa.xp@gmail.com) Check if overflow of unsigned exists at all
//
// All signed binop checkers share one ABI, mirroring how UBSan's
// __ubsan_handle_*_overflow handlers take the operand type as a parameter:
//
//   (long long lhs, long long rhs, unsigned width, ...)
//
// OverflowPass sign-extends both operands to 64 bits (lossless) and passes
// `width`, the bit width of the *result type of the operation*. SV-COMP's
// no-overflow property is defined over exactly that type — "the resulting type
// of an operation is a signed-integer type but the resulting value is not in
// the range of values that are representable by that type" — so the check has
// to range-check the exact result against `width`, not against a fixed width.
// Checking only at 64 bits would mask every int overflow; checking only at 32
// bits would miss every long long one (Juliet CWE190/CWE191 has both).

/**
 * @brief Checks ADD (on two integers) operations and adds cases where operation
 * would fail.
 * @param param1 First operand, sign-extended to 64 bits
 * @param param2 Second operand, sign-extended to 64 bits
 * @param width Bit width of the operation's result type
 * @param line Line where operation occurred
 * @param scope Scope where operation occurred
 * @param function_name Name of the function which operation occurred
 */
void map2check_binop_add(long long param1, long long param2, unsigned width,
                         unsigned line, unsigned scope, char *function_name);

void map2check_binop_add_uint(unsigned long long param1,
                              unsigned long long param2, unsigned width,
                              unsigned line, unsigned scope,
                              char *function_name);
/**
 * @brief Checks SUB (on two integers) operations and adds cases where operation
 * would fail.
 * @param param1 First operand, sign-extended to 64 bits
 * @param param2 Second operand, sign-extended to 64 bits
 * @param width Bit width of the operation's result type
 * @param line Line where operation occurred
 * @param scope Scope where operation occurred
 * @param function_name Name of the function which operation occurred
 * */
void map2check_binop_sub(long long param1, long long param2, unsigned width,
                         unsigned line, unsigned scope, char *function_name);

void map2check_binop_sub_uint(unsigned long long param1,
                              unsigned long long param2, unsigned width,
                              unsigned line, unsigned scope,
                              char *function_name);

/**
 * @brief Checks MUL (on two integers) operations and adds cases where operation
 * would fail.
 * @param param1 First operand, sign-extended to 64 bits
 * @param param2 Second operand, sign-extended to 64 bits
 * @param width Bit width of the operation's result type
 * @param line Line where operation occurred
 * @param scope Scope where operation occurred
 * @param function_name Name of the function which operation occurred
 * */
void map2check_binop_mul(long long param1, long long param2, unsigned width,
                         unsigned line, unsigned scope, char *function_name);

void map2check_binop_mul_uint(unsigned long long param1,
                              unsigned long long param2, unsigned width,
                              unsigned line, unsigned scope,
                              char *function_name);

/**
 * @brief Checks DIV (on two integers) operations and adds cases where operation
 * would fail.
 * @param param1 Dividend, sign-extended to 64 bits
 * @param param2 Divisor, sign-extended to 64 bits
 * @param width Bit width of the operation's result type
 * @param line Line where operation occurred
 * @param scope Scope where operation occurred
 * @param function_name Name of the function which operation occurred
 * */
void map2check_binop_sdiv(long long param1, long long param2, unsigned width,
                          unsigned line, unsigned scope, char *function_name);

/**
 * @brief Checks REM (on two integers) operations. Same failure modes as SDIV:
 * a zero divisor, and MIN % -1 which is undefined in C.
 * @param param1 Dividend, sign-extended to 64 bits
 * @param param2 Divisor, sign-extended to 64 bits
 * @param width Bit width of the operation's result type
 * @param line Line where operation occurred
 * @param scope Scope where operation occurred
 * @param function_name Name of the function which operation occurred
 * */
void map2check_binop_srem(long long param1, long long param2, unsigned width,
                          unsigned line, unsigned scope, char *function_name);

/* Lowest value representable by a signed integer type of `width` bits. */
static long long signed_min_for_width(unsigned width) {
  if (width == 0 || width >= 64) {
    return LLONG_MIN;
  }
  return -(1LL << (width - 1));
}

/* Highest value representable by a signed integer type of `width` bits. */
static long long signed_max_for_width(unsigned width) {
  if (width == 0 || width >= 64) {
    return LLONG_MAX;
  }
  return (1LL << (width - 1)) - 1;
}

static Bool fits_in_width(long long value, unsigned width) {
  if (value < signed_min_for_width(width)) {
    return FALSE;
  }
  if (value > signed_max_for_width(width)) {
    return FALSE;
  }
  return TRUE;
}

/*
 * 64-bit overflow predicates. These are written as range comparisons rather
 * than by computing the result first, because computing it would itself be the
 * undefined behaviour we are trying to report.
 */
static Bool add_overflows_64(long long a, long long b) {
  if (b > 0 && a > LLONG_MAX - b) {
    return TRUE;
  }
  if (b < 0 && a < LLONG_MIN - b) {
    return TRUE;
  }
  return FALSE;
}

static Bool sub_overflows_64(long long a, long long b) {
  if (b < 0 && a > LLONG_MAX + b) {
    return TRUE;
  }
  if (b > 0 && a < LLONG_MIN + b) {
    return TRUE;
  }
  return FALSE;
}

static Bool mul_overflows_64(long long a, long long b) {
  if (a == 0 || b == 0) {
    return FALSE;
  }
  /* Handled first so the divisions below never evaluate LLONG_MIN / -1. */
  if (a == -1) {
    return (b == LLONG_MIN) ? TRUE : FALSE;
  }
  if (b == -1) {
    return (a == LLONG_MIN) ? TRUE : FALSE;
  }
  if (a > 0) {
    if (b > 0) {
      return (a > LLONG_MAX / b) ? TRUE : FALSE;
    }
    return (b < LLONG_MIN / a) ? TRUE : FALSE;
  }
  if (b > 0) {
    return (a < LLONG_MIN / b) ? TRUE : FALSE;
  }
  /* Both negative: the product is positive. */
  return (a < LLONG_MAX / b) ? TRUE : FALSE;
}

void overflowError(unsigned line, const char *function_name) {
  write_property(FALSE_OVERFLOW, line, function_name);
  map2check_error();
}

void unknown_not_supported() { write_property_unknown(); }

void divisionByZeroError(unsigned line, const char *function_name) {
  write_property(FALSE_DIVBYZERO, line, function_name);
  map2check_error();
}

void map2check_binop_add(long long param1, long long param2, unsigned width,
                         unsigned line, unsigned scope, char *function_name) {
  if (add_overflows_64(param1, param2)) {
    overflowError(line, function_name);
    return;
  }
  if (!fits_in_width(param1 + param2, width)) {
    overflowError(line, function_name);
  }
}

void map2check_binop_sub(long long param1, long long param2, unsigned width,
                         unsigned line, unsigned scope, char *function_name) {
  if (sub_overflows_64(param1, param2)) {
    overflowError(line, function_name);
    return;
  }
  if (!fits_in_width(param1 - param2, width)) {
    overflowError(line, function_name);
  }
}

/*
 * Unsigned wraparound is well-defined in C and is not an overflow under
 * SV-COMP's no-overflow property, so these remain deliberate no-ops. They are
 * kept only so the pass has a symbol to reference.
 */
void map2check_binop_add_uint(unsigned long long param1,
                              unsigned long long param2, unsigned width,
                              unsigned line, unsigned scope,
                              char *function_name) {}

void map2check_binop_sub_uint(unsigned long long param1,
                              unsigned long long param2, unsigned width,
                              unsigned line, unsigned scope,
                              char *function_name) {}

void map2check_binop_mul(long long param1, long long param2, unsigned width,
                         unsigned line, unsigned scope, char *function_name) {
  if (mul_overflows_64(param1, param2)) {
    overflowError(line, function_name);
    return;
  }
  if (!fits_in_width(param1 * param2, width)) {
    overflowError(line, function_name);
  }
}

void map2check_binop_mul_uint(unsigned long long param1,
                              unsigned long long param2, unsigned width,
                              unsigned line, unsigned scope,
                              char *function_name) {}

void map2check_binop_sdiv(long long param1, long long param2, unsigned width,
                          unsigned line, unsigned scope, char *function_name) {
  if (param2 == 0) {
    divisionByZeroError(line, function_name);
    return;
  }

  /* MIN / -1 is the only signed division that overflows: the magnitude of the
   * quotient is one past the representable maximum. */
  if (param1 == signed_min_for_width(width) && param2 == -1) {
    overflowError(line, function_name);
  }
}

void map2check_binop_srem(long long param1, long long param2, unsigned width,
                          unsigned line, unsigned scope, char *function_name) {
  if (param2 == 0) {
    divisionByZeroError(line, function_name);
    return;
  }

  /* MIN % -1 is mathematically 0, but C leaves it undefined because it is
   * specified in terms of the corresponding division. */
  if (param1 == signed_min_for_width(width) && param2 == -1) {
    overflowError(line, function_name);
  }
}

// Overflow mode does not use any additional data
void analysis_init() {}

void analysis_destroy() {}

Bool analysis_is_program_correct() { return TRUE; }

void analysis_generate_aux_witness_files() {}
