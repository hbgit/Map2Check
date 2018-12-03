#include "../header/HeapLog.h"
#include <stdio.h>
#include <string.h>

const char* heap_log_file = "heap_log.csv";
Bool mark_heap_log(MAP2CHECK_CONTAINER* heap_log, MEMORY_HEAP_ROW row) {
  if (heap_log->type != HEAP_LOG_CONTAINER) {
    return FALSE;
  }

  return append_element(heap_log, &row);
}

MEMORY_HEAP_ROW new_heap_row(int line, int scope, void* address, int size,
                             int size_of_primitive, const char* function_name) {
  MEMORY_HEAP_ROW row;
  strncpy(row.function_name, function_name, FUNCTION_MAX_LENGTH_NAME);
  row.line = line;
  row.scope = scope;
  row.value = address;
  row.size = size;
  row.size_of_primitive = size_of_primitive;
  return row;
}

/* Same idea of is_valid_allocation_address from AllocationLog.c,
 * but the main difference is: there is no need to check if address is free
 */
Bool is_valid_heap_address(MAP2CHECK_CONTAINER* heap_log, void* address,
                           int size_to_load) {
  int i = heap_log->size - 1;
  long addressToCheck = (long)address;
  for (; i >= 0; i--) {
    MEMORY_HEAP_ROW* iRow = (MEMORY_HEAP_ROW*)get_element_at(i, *heap_log);
    long addressBottom = (long)iRow->value;
    long addressTop = addressBottom + iRow->size - size_to_load + 1;
    if ((addressBottom <= addressToCheck) && (addressToCheck < addressTop)) {
      //*last_address = addressTop;
      free_used_element(iRow);
      return TRUE;
    }
    free_used_element(iRow);
  }
  //*last_address = 0;
  return FALSE;
}

void heap_log_to_file(MAP2CHECK_CONTAINER* list) {
  FILE* output = fopen(heap_log_file, "w");
  // fprintf(output, "id;memory address;points to;scope;is free;is
  // dynamic;function name\n");
  int i = 0;
  for (; i < list->size; i++) {
    MEMORY_HEAP_ROW* row = (MEMORY_HEAP_ROW*)get_element_at(i, *list);
    fprintf(output, "%p;", (void*)((long)row->value));
    fprintf(output, "%s;", row->function_name);
    fprintf(output, "%d\n", row->size);
    free_used_element(row);
  }
  fclose(output);
}
