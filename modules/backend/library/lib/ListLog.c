#include "../header/ListLog.h"

#include <stdio.h>
#include <string.h>
const char* list_log_file = "list_log.csv";

long get_old_reference(long var_address, MAP2CHECK_CONTAINER* log) {
  int i = log->size - 1;
  for (; i >= 0; i--) {
    LIST_LOG_ROW* row = (LIST_LOG_ROW*)get_element_at(i, *log);

    if (row->memory_address == var_address) {
      long result = row->memory_address_points_to;
      free_used_element(row);
      return result;
    }
  }

  return 0;
}

/*
  A memcleanup error occurs when a memory leak happens but we still have
  a pointer that points to the leaked location, so to verify we:
  1. From last element to the first check if some variable points to the
  location
  2. If found, iterates from the found element to the last and check if the
  pointer does not change.
  3a. If it doesn't change, then it is a memcleanup error
  3b. Else, go on 1 loop.
  4. If loop ends without finding leaked address, then it isn't a memcleanup
  error.
*/
Bool is_memcleanup_error(long address, MAP2CHECK_CONTAINER* log) {
  int i = log->size - 1;
  // 1
  for (; i >= 0; i--) {
    LIST_LOG_ROW* row = (LIST_LOG_ROW*)get_element_at(i, *log);

    long points_to = row->memory_address_points_to;

    if (points_to == address) {
      long pointer_address = row->memory_address;
      int j = log->size - 1;
      Bool error = TRUE;
      for (; j > i; j--) {
        LIST_LOG_ROW* row_j = (LIST_LOG_ROW*)get_element_at(j, *log);
        if (pointer_address == row_j->memory_address) {
          error = row_j->memory_address_points_to == address ? TRUE : FALSE;
          free_used_element(row_j);
          break;
        }
        free_used_element(row_j);
      }
      if (error) {
        free_used_element(row);
        return TRUE;
      }
    }
    free_used_element(row);
  }
  return FALSE;
}
// TODO: Implement method
Bool is_deref_error(long address, MAP2CHECK_CONTAINER* log) {
  int i = log->size - 1;

  for (; i >= 0; i--) {
    LIST_LOG_ROW* row = (LIST_LOG_ROW*)get_element_at(i, *log);

    long points_to = row->memory_address_points_to;
    long address_origin = row->memory_address;

    if (address_origin == address) {
      return FALSE;
    }

    if (points_to == address) {
      Bool is_free = row->is_free;
      Bool is_dynamic = row->is_dynamic;
      free_used_element(row);
      if (is_free || (!is_dynamic)) {
        return TRUE;
      } else {
        return FALSE;
      }
    }
    free_used_element(row);
  }

  return TRUE;
}

Bool is_invalid_free(long address, MAP2CHECK_CONTAINER* log) {
  int i = log->size - 1;
  if (address == ((long)NULL)) {
    return FALSE;
  }

  for (; i >= 0; i--) {
    LIST_LOG_ROW* row = (LIST_LOG_ROW*)get_element_at(i, *log);
    long points_to = row->memory_address_points_to;

    if (points_to == address) {
      Bool is_free = row->is_free;
      Bool is_dynamic = row->is_dynamic;
      free_used_element(row);
      if (is_free || (!is_dynamic)) {
        return TRUE;
      } else {
        return FALSE;
      }
    }
    free_used_element(row);
  }

  return TRUE;
}

LIST_LOG_ROW new_list_row(long memory_address, long memory_address_points_to,
                          unsigned scope, Bool is_dynamic, Bool is_free,
                          unsigned line_number, const char* name,
                          const char* function_name, unsigned step) {
  LIST_LOG_ROW row;
  row.id = 0;
  row.is_dynamic = is_dynamic;
  row.is_free = is_free;
  row.line_number = line_number;
  row.memory_address = memory_address;
  row.memory_address_points_to = memory_address_points_to;
  row.scope = scope;
  row.var_name = name;
  strncpy(row.function_name, function_name, FUNCTION_MAX_LENGTH_NAME);
  row.step_on_execution = step;
  return row;
}

void list_log_to_file(MAP2CHECK_CONTAINER* list) {
  FILE* output = fopen(list_log_file, "w");
  // fprintf(output, "id;memory address;points to;scope;is free;is
  // dynamic;function name\n");
  int i = 0;
  for (; i < list->size; i++) {
    LIST_LOG_ROW* row = (LIST_LOG_ROW*)get_element_at(i, *list);
    fprintf(output, "%d;", row->id);
    fprintf(output, "%p;", (void*)row->memory_address);
    fprintf(output, "%p;", (void*)row->memory_address_points_to);
    fprintf(output, "%d;", row->scope);
    fprintf(output, "%d;", row->is_free);
    fprintf(output, "%d;", row->is_dynamic);
    fprintf(output, "%s;", row->var_name);
    fprintf(output, "%d;", row->line_number);
    fprintf(output, "%s;", row->function_name);
    fprintf(output, "%d\n", row->step_on_execution);
    free_used_element(row);
  }
  fclose(output);
}

enum MemoryAddressStatus get_type_from_list_log_row(LIST_LOG_ROW* row) {
  if (row->is_free && !row->is_dynamic) {
    return FREE;
  } else if (row->is_dynamic) {
    return DYNAMIC;
  }

  // FIXME: how to known if address if static or invalid?
  return STATIC;
}
