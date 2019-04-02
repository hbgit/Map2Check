#include "../header/NonDetLog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../header/Map2CheckFunctions.h"

const char *klee_log_file = "klee_log.csv";

Bool nondet_log_to_file(MAP2CHECK_CONTAINER klee_container) {
  if (klee_container.type != NONDET_LOG_CONTAINER) {
    return FALSE;
  }

  FILE *output = fopen(klee_log_file, "w");

  int i = 0;
  unsigned size = klee_container.size;
  for (; i < size; i++) {
    NONDET_CALL *call = (NONDET_CALL *)get_element_at(i, klee_container);
    if (call == NULL) {
      return FALSE;
    }
    fprintf(output, "%d;", i);
    fprintf(output, "%d;", call->line);
    fprintf(output, "%d;", call->scope);
    fprintf(output, "%s;", call->function_name);
    fprintf(output, "%d;", call->step_on_execution);

    if (((int)call->type) == UNSIGNED) {
      fprintf(output, "%u;", (unsigned)(call->value));  // TODO for unsigned
    } else {
      fprintf(output, "%d;", ((int)call->value));  // TODO for unsigned
    }

    // printf("%u \n;", call->value);
    fprintf(output, "%d\n", ((int)call->type));
  }
  fclose(output);
  return TRUE;
}

NONDET_CALL new_nondet_call(enum NONDET_TYPE type, unsigned line,
                            unsigned scope, long value,
                            const char *function_name, unsigned step) {
  NONDET_CALL result;
  result.type = type;
  strncpy(result.function_name, function_name, FUNCTION_MAX_LENGTH_NAME);
  result.line = line;
  result.value = value;
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

  NONDET_CALL *row = (NONDET_CALL *)malloc(sizeof(NONDET_CALL));
  *row = nondetCall;

  append_element(&nondet_log, row);
}

#define MAP2CHECK_NONDET_GENERATOR(type, nondet_type)                    \
  void map2check_nondet_##type(unsigned line, unsigned scope, int value, \
                               const char *function_name) {              \
    NONDET_CALL nondetCall =                                             \
        new_nondet_call(nondet_type, line, scope, value, function_name,  \
                        map2check_get_current_step());                   \
    helper_map2check_nondet_append_element(nondetCall);                  \
  }

MAP2CHECK_NONDET_GENERATOR(int, INTEGER)
MAP2CHECK_NONDET_GENERATOR(char, CHAR)
MAP2CHECK_NONDET_GENERATOR(pointer, POINTER)
MAP2CHECK_NONDET_GENERATOR(ushort, USHORT)
MAP2CHECK_NONDET_GENERATOR(long, LONG)
MAP2CHECK_NONDET_GENERATOR(ulong, ULONG)
MAP2CHECK_NONDET_GENERATOR(bool, BOOL)
MAP2CHECK_NONDET_GENERATOR(uchar, UCHAR)
MAP2CHECK_NONDET_GENERATOR(pchar, PCHAR)
MAP2CHECK_NONDET_GENERATOR(size_t, SIZE_T)
MAP2CHECK_NONDET_GENERATOR(loff_t, LOFF_T)
MAP2CHECK_NONDET_GENERATOR(sector_t, SECTOR_T)
MAP2CHECK_NONDET_GENERATOR(uint, UINT)
// FIX: MAP2CHECK_NONDET_GENERATOR(unsigned, UNSIGNED)

void map2check_nondet_unsigned(unsigned line, unsigned scope, unsigned value,
                               const char *function_name) {
  NONDET_CALL nondetCall =
      new_nondet_call(UNSIGNED, line, scope, value, function_name,
                      map2check_get_current_step());
  helper_map2check_nondet_append_element(nondetCall);
}
