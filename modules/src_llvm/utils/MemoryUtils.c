#include <stdio.h>
#include "MemoryUtils.h"

#define DEBUG

LIST_LOG list_map2check;
bool list_initialized = false;

void list_log_to_file(LIST_LOG_ROW* row) {
  FILE* output = fopen("output", "a");

  fprintf(output, "ID, %d\n", row->id);
  fprintf(output, "\tMEMORY ADDRESS, %p\n", row->memory_address);
  fprintf(output, "\tPOINTS TO, %p\n", row->memory_address_points_to);
  fprintf(output, "\tSCOPE, %s", row->scope);
  fprintf(output, "\tIS FREE, %d\n", row->is_free);
  fprintf(output, "\tIS DYNAMIC, %d\n", row->is_dynamic);
  fprintf(output, "\n\n");

  fclose(output);
}


void map2check_pointer(void* x, const   char* name){
  // MemoryRow* row = new MemoryRow((long) x, 0, "foo", true, false);
  if(list_initialized == false) {
    list_map2check = new_list_log();
    list_initialized = true;
  }


  LIST_LOG_ROW row = new_list_row((long) x, 0, "foo", true, false, __LINE__,name);


  mark_map_log(&list_map2check, &row);

}

LIST_LOG_ROW new_list_row (long memory_address, long memory_address_points_to,
  const char* scope, bool is_dynamic, bool is_free, unsigned line_number, const char* name) {
    LIST_LOG_ROW row;
    row.id = 0;
    row.is_dynamic = is_dynamic;
    row.is_free = is_free;
    row.line_number = line_number;
    row.memory_address = memory_address;
    row.memory_address_points_to = memory_address_points_to;
    row.scope = scope;
    row.var_name = name;

    #ifdef DEBUG
    list_log_to_file(&row);

    printf("Adding new log to LIST_LOG:\n");
    printf("\t MEMORY ADDRESS: %p\n", row.memory_address);
    printf("\t MEMORY ADDRESS POINTS TO: %p\n", row.memory_address_points_to);
    printf("\t SCOPE: %s\n", row.scope);
    printf("\t IS DYNAMIC: %d\n", row.is_dynamic);
    printf("\t IS FREE: %d\n", row.is_free);
    printf("\t VAR_NAME: %s\n", row.var_name);
    printf("\t LINE NUMBER: %d\n", row.line_number);
    printf("\n");
    #endif

    return row;
}

LIST_LOG new_list_log() {
  LIST_LOG list;

  list.size = 0;
  list.values = 0;

  #ifdef DEBUG
  printf("Creating new LIST_LOG");
  printf("\n\n");
  #endif

  return list;
}

bool mark_map_log(LIST_LOG* list, LIST_LOG_ROW* row) {
  list->size++;

  LIST_LOG_ROW* temp_list = (LIST_LOG_ROW*) realloc (list->values  , list->size * sizeof(LIST_LOG_ROW));
  list->values = temp_list;

  list->values[list->size-1] = *row;
  list->values[list->size-1].id = list->size;

  #ifdef DEBUG
  printf("Marking new element to LIST_LOG");
  printf("\n LIST_LOG SIZE: %d", list->size);
  printf("\n\n");
  #endif

  return true;
}

void map2check_free_list_log(){
  free_list_log(&list_map2check);
}

bool free_list_log(LIST_LOG* list) {
    // free(list);
  #ifdef DEBUG
  printf("Freeing LIST_LOG values");
  printf("\n\n");
  #endif
  free(list->values);
  return true;
}
