/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include "../header/NonDetLog.h"
#include "../header/Map2CheckFunctions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *klee_log_file = "klee_log.csv";

/* Decimal conversion by hand, not through printf. Under KLEE, uClibc's
 * vfprintf is interpreted instruction by instruction -- about 7,000 per call,
 * seven calls per row -- and the log of a run with 1,000 nondet reads cost
 * 44 M instructions, 93% of the whole path (issue #75). Each helper appends
 * to `buffer` and returns the new end. */
static char *put_ulonglong(char *buffer, unsigned long long value) {
  char digits[20];
  int n = 0;
  do {
    digits[n++] = (char)('0' + value % 10);
    value /= 10;
  } while (value != 0);
  while (n > 0)
    *buffer++ = digits[--n];
  return buffer;
}

static char *put_longlong(char *buffer, long long value) {
  if (value < 0) {
    *buffer++ = '-';
    /* negate in unsigned: -LLONG_MIN overflows */
    return put_ulonglong(buffer, 0ULL - (unsigned long long)value);
  }
  return put_ulonglong(buffer, (unsigned long long)value);
}

static char *put_string(char *buffer, const char *text, size_t max) {
  size_t i;
  for (i = 0; i < max && text[i] != '\0'; i++)
    *buffer++ = text[i];
  return buffer;
}

Bool nondet_log_to_file(MAP2CHECK_CONTAINER klee_container) {
  if (klee_container.type != NONDET_LOG_CONTAINER) {
    return FALSE;
  }

  FILE *output = fopen(klee_log_file, "w");
  if (output == NULL) {
    return FALSE;
  }

  /* id;line;scope;function;step;value;type -- five numbers of at most 20
   * characters, the name, a double of at most ~320 (%lf of DBL_MAX) */
  char row[512];
  unsigned i = 0;
  unsigned size = klee_container.size;
  for (; i < size; i++) {
    NONDET_CALL *call = (NONDET_CALL *)get_element_at(i, klee_container);
    if (call == NULL) {
      fclose(output);
      return FALSE;
    }
    char *end = row;
    end = put_longlong(end, (int)i);
    *end++ = ';';
    end = put_ulonglong(end, call->line);
    *end++ = ';';
    end = put_ulonglong(end, call->scope);
    *end++ = ';';
    end = put_string(end, call->function_name, FUNCTION_MAX_LENGTH_NAME);
    *end++ = ';';
    end = put_ulonglong(end, call->step_on_execution);
    *end++ = ';';

    // The value generated from nondet function. Read inline: it is stored in
    // the row, not behind a pointer into a stack frame that no longer exists.
    /* else-if, not two independent ifs. The UNSIGNED arm used to be an `if`
     * with no `else`, so an unsigned row fell through into the `%d` arm as
     * well and printed its value TWICE: `0;3;17;main;0;5;5;5`, eight fields
     * where the format has seven. The value column still parsed by accident,
     * being first, but the type column moved to index 7 and anything reading
     * it got a copy of the value instead. */
    if (((int)call->type) == UNSIGNED || ((int)call->type) == U32) {
      end = put_ulonglong(end, call->value.as_unsigned);
    } else if (((int)call->type) == DOUBLE) {
      end += snprintf(end, row + sizeof(row) - end - 16, "%lf",
                      call->value.as_double);
    } else if (((int)call->type) == FLOAT) {
      /* %.9g, not %lf: nine significant digits round-trip any float, while
       * six fixed decimals print 1e-30 as 0 -- a different input. */
      end += snprintf(end, row + sizeof(row) - end - 16, "%.9g",
                      (float)call->value.as_double);
    } else if (((int)call->type) == LONGLONG) {
      end = put_longlong(end, call->value.as_longlong);
    } else if (((int)call->type) == ULONGLONG) {
      end = put_ulonglong(end, call->value.as_ulonglong);
    } else {
      end = put_longlong(end, call->value.as_int);
    }
    *end++ = ';';

    // Type of the nondet function
    end = put_longlong(end, (int)call->type);
    *end++ = '\n';
    fwrite(row, 1, (size_t)(end - row), output);
  }

  fclose(output);
  return TRUE;
}

NONDET_CALL new_nondet_call(enum NONDET_TYPE type, unsigned line,
                            unsigned scope, void *value,
                            const char *function_name, unsigned step) {
  NONDET_CALL result;
  /* zero-initialize: id and padding are persisted when the row is written
   * to disk by the BTree container */
  memset(&result, 0, sizeof(result));
  result.type = type;
  strncpy(result.function_name, function_name, FUNCTION_MAX_LENGTH_NAME);
  result.line = line;
  /* Copy the value in NOW, while the caller's storage is still alive. Keeping
   * the pointer instead is what made every symbolic run lose its nondet log:
   * `value` points at a parameter of the map2check_nondet_* wrapper, and that
   * frame is gone long before the log is flushed at exit. */
  if (value != NULL) {
    if (type == DOUBLE || type == FLOAT) {
      result.value.as_double = *(double *)value;
    } else if (type == LONGLONG || type == ULONGLONG) {
      result.value.as_longlong = *(long long *)value;
    } else {
      result.value.as_int = *(int *)value;
    }
  }
  result.scope = scope;
  result.step_on_execution = step;
  return result;
}

MAP2CHECK_CONTAINER nondet_log;

void nondet_log_init() { nondet_log = new_container(NONDET_LOG_CONTAINER); }

void nondet_log_destroy() { free_container(&nondet_log); }

MAP2CHECK_CONTAINER map2check_nondet_get_log() { return nondet_log; }

void helper_map2check_nondet_append_element(NONDET_CALL nondetCall) {
  map2check_next_current_step();

  /* append_element copies the row: a heap copy here leaked one row per
   * nondet read (and was one more memory object per read under KLEE). */
  append_element(&nondet_log, &nondetCall);
}

#define MAP2CHECK_NONDET_GENERATOR(type, nondet_type)                          \
  void map2check_nondet_##type(unsigned line, unsigned scope, int value,       \
                               const char *function_name) {                    \
    void *tmp_void_int = &value;                                               \
    NONDET_CALL nondetCall =                                                   \
        new_nondet_call(nondet_type, line, scope, tmp_void_int, function_name,        \
                        map2check_get_current_step());                         \
    helper_map2check_nondet_append_element(nondetCall);                        \
  }

MAP2CHECK_NONDET_GENERATOR(int, INTEGER)
MAP2CHECK_NONDET_GENERATOR(char, CHAR)
MAP2CHECK_NONDET_GENERATOR(pointer, POINTER)
MAP2CHECK_NONDET_GENERATOR(ushort, USHORT)
MAP2CHECK_NONDET_GENERATOR(short, SHORT)
MAP2CHECK_NONDET_GENERATOR(long, LONG)
MAP2CHECK_NONDET_GENERATOR(ulong, ULONG)
MAP2CHECK_NONDET_GENERATOR(bool, BOOL)
MAP2CHECK_NONDET_GENERATOR(uchar, UCHAR)
MAP2CHECK_NONDET_GENERATOR(pchar, PCHAR)
MAP2CHECK_NONDET_GENERATOR(size_t, SIZE_T)
MAP2CHECK_NONDET_GENERATOR(loff_t, LOFF_T)
MAP2CHECK_NONDET_GENERATOR(sector_t, SECTOR_T)
MAP2CHECK_NONDET_GENERATOR(uint, UINT)
MAP2CHECK_NONDET_GENERATOR(u8, U8)
MAP2CHECK_NONDET_GENERATOR(u16, U16)
MAP2CHECK_NONDET_GENERATOR(u32, U32)
MAP2CHECK_NONDET_GENERATOR(memory_byte, MEMORY_BYTE)
// FIX: MAP2CHECK_NONDET_GENERATOR(unsigned, UNSIGNED)

void map2check_nondet_unsigned(unsigned line, unsigned scope, unsigned value,
                               const char *function_name) {
  // TODO: save unsigned into ptr void
  void *tmp_void_uint = &value;
  NONDET_CALL nondetCall =
      new_nondet_call(UNSIGNED, line, scope, tmp_void_uint, function_name,
                      map2check_get_current_step());
  helper_map2check_nondet_append_element(nondetCall);
}

void map2check_nondet_double(unsigned line, unsigned scope, double value,
                               const char *function_name) {
  /* DOUBLE, not UNSIGNED. Tagging a double as unsigned made the writer read
   * the first four bytes of the mantissa and print them with %u, so every
   * logged floating-point input was nonsense. */
  NONDET_CALL nondetCall =
      new_nondet_call(DOUBLE, line, scope, &value, function_name,
                      map2check_get_current_step());
  helper_map2check_nondet_append_element(nondetCall);
}

/* A float arrives widened to double (NonDetPass emits an fpext), so the row
 * keeps one floating-point member; FLOAT says how to print it. */
void map2check_nondet_float(unsigned line, unsigned scope, double value,
                            const char *function_name) {
  NONDET_CALL nondetCall =
      new_nondet_call(FLOAT, line, scope, &value, function_name,
                      map2check_get_current_step());
  helper_map2check_nondet_append_element(nondetCall);
}

/* 64-bit, not through the int-valued macro above: that one truncates to 32
 * bits, and an Intel-TDX register value cut in half is a different input. */
void map2check_nondet_longlong(unsigned line, unsigned scope, long long value,
                               const char *function_name) {
  NONDET_CALL nondetCall =
      new_nondet_call(LONGLONG, line, scope, &value, function_name,
                      map2check_get_current_step());
  helper_map2check_nondet_append_element(nondetCall);
}

void map2check_nondet_ulonglong(unsigned line, unsigned scope,
                                unsigned long long value,
                                const char *function_name) {
  NONDET_CALL nondetCall =
      new_nondet_call(ULONGLONG, line, scope, &value, function_name,
                      map2check_get_current_step());
  helper_map2check_nondet_append_element(nondetCall);
}
