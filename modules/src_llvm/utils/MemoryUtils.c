#include <stdio.h>
#include <klee/klee.h>
#include "MemoryUtils.h"

// #define  DEBUG

MEMORY_ALLOCATIONS_ROW new_memory_row(long address, bool is_free) {
  MEMORY_ALLOCATIONS_ROW row;
  row.addr = address;
  row.is_free = is_free;
  return row;
}

MEMORY_ALLOCATIONS_LOG new_memory_allocation() {
  MEMORY_ALLOCATIONS_LOG log_list;
  log_list.values = NULL;
  log_list.size   = 0;
  return log_list;
}

LIST_LOG list_map2check;
bool list_initialized = false;

MEMORY_ALLOCATIONS_LOG allocations_map2check;
bool allocations_initialized = false;

bool mark_allocation_log(MEMORY_ALLOCATIONS_LOG* allocation_log, long address) {
  MEMORY_ALLOCATIONS_ROW row = new_memory_row(address, false);
  allocation_log->size++;

  MEMORY_ALLOCATIONS_ROW* temp_list = (MEMORY_ALLOCATIONS_ROW*)
    realloc (allocation_log->values,
	     allocation_log->size * sizeof(MEMORY_ALLOCATIONS_ROW));

  allocation_log->values = temp_list;
  allocation_log->values[allocation_log->size-1] = row;

  return true;
}

bool mark_deallocation_log(MEMORY_ALLOCATIONS_LOG* allocation_log, long address) {
  MEMORY_ALLOCATIONS_ROW row = new_memory_row(address, true);

  allocation_log->size++;

  MEMORY_ALLOCATIONS_ROW* temp_list = (MEMORY_ALLOCATIONS_ROW*)
    realloc (allocation_log->values,
	     allocation_log->size * sizeof(MEMORY_ALLOCATIONS_ROW));
  
  allocation_log->values = temp_list;  //
  allocation_log->values[allocation_log->size-1] = row;

  return true;
}

MEMORY check_address_allocation_log(MEMORY_ALLOCATIONS_LOG* allocation_log, long address) {
  int i = allocation_log->size - 1;

  for(i; i >= 0; i--) {
    if(allocation_log->values[i].addr == address) {
      if(allocation_log->values[i].is_free == true) {
        return FREE;
      }
      else {
        return DYNAMIC;
      }
    }
  }

  return STATIC;

}

bool free_memory_allocation_log(MEMORY_ALLOCATIONS_LOG* allocation_log) {
#ifdef DEBUG
  print_allocation_log(allocation_log);
#endif
  free(allocation_log->values);
  return true;

}

void print_allocation_log(MEMORY_ALLOCATIONS_LOG* allocation_log) {
  printf("\n#############################################################################\n");
  printf("################## ALLOCATION LOG ###########################################  \n");
  printf("#############################################################################  \n");

  int i = 0;
  for(;i < allocation_log->size;i++) {
    MEMORY_ALLOCATIONS_ROW row = allocation_log->values[i];
    printf("\t# ID: %d \n", i);
    printf("\t# MEMORY ADDRESS: %p \n", row.addr);
    printf("\t# IS FREE: %d \n", row.is_free);
    printf("\n");
  }
}
void map2check_add_store_pointer(void* var, void* value,unsigned scope, const char* name, int line) {
  if(list_initialized == false) {
    list_map2check = new_list_log();
    list_initialized = true;
  }

  if(!allocations_initialized) {
    allocations_map2check = new_memory_allocation();
    allocations_initialized = true;
  }

  MEMORY status = check_address_allocation_log(&allocations_map2check, (long) value);
  bool isDynamic;
  bool isFree;

  switch (status) {
  case STATIC:
    isDynamic = false;
    isFree = false;
    break;
  case FREE:
    isDynamic = false;
    isFree = true;
    break;
  case DYNAMIC:
    isDynamic = true;
    isFree = false;
    break;
  }

  LIST_LOG_ROW row = new_list_row((long) var,(long) value, scope, isDynamic, isFree, line, name);
  mark_map_log(&list_map2check, &row);
}

void map2check_pointer(void* x,unsigned scope, const char* name, int line){
  if(list_initialized == false) {
    list_map2check = new_list_log();
    list_initialized = true;
  }

  LIST_LOG_ROW row = new_list_row((long) x, 0, scope, true, false, line,name);
  mark_map_log(&list_map2check, &row);
}

void list_log_to_file(LIST_LOG* list) {
  FILE* output = fopen("list_log", "w");
  int i = 0;
  for(;i< list->size; i++) {
    LIST_LOG_ROW*row = &list->values[i];
    fprintf(output, "%d,", row->id);
    fprintf(output, "%p,", row->memory_address);
    fprintf(output, "%p,", row->memory_address_points_to);
    fprintf(output, "%d,", row->scope);
    fprintf(output, "%d,", row->is_free);
    fprintf(output, "%d\n", row->is_dynamic);

  }

  fclose(output);
}

LIST_LOG_ROW new_list_row (long memory_address, long memory_address_points_to,
			   unsigned scope, bool is_dynamic, bool is_free, unsigned line_number, const char* name) {
  LIST_LOG_ROW row;
  row.id = 0;
  row.is_dynamic = is_dynamic;
  row.is_free = is_free;
  row.line_number = line_number;
  row.memory_address = memory_address;
  row.memory_address_points_to = memory_address_points_to;
  row.scope = scope;
  row.var_name = name;

  return row;
}

LIST_LOG new_list_log() {
  LIST_LOG list;

  list.size = 0;
  list.values = 0;

  return list;
}

bool mark_map_log(LIST_LOG* list, LIST_LOG_ROW* row) {
  list->size++;

  LIST_LOG_ROW* temp_list = (LIST_LOG_ROW*)
    realloc (list->values,
	     list->size * sizeof(LIST_LOG_ROW));
  
  list->values = temp_list;

  list->values[list->size-1] = *row;
  list->values[list->size-1].id = list->size;

  return true;
}

void map2check_free_list_log(){
  free_list_log(&list_map2check);
  free_memory_allocation_log(&allocations_map2check);
}

bool free_list_log(LIST_LOG* list) {
  // free(list);

  // print_list_log(&list_map2check);
  list_log_to_file(&list_map2check);
  free(list->values);
  return true;
}

void print_list_log(LIST_LOG* list) {
  printf("\n#############################################################################\n");
  printf("################## LIST LOG #################################################  \n");
  printf("#############################################################################  \n");

  int i =0;
  for(;i<list->size;i++) {
    LIST_LOG_ROW row = list->values[i];
    printf("\t# ID: %d \n", row.id);
    printf("\t# MEMORY ADDRESS: %p \n", row.memory_address);
    printf("\t# POINTS TO: %p \n", row.memory_address_points_to);
    printf("\t# SCOPE: %d\n", row.scope);
    printf("\t# IS DYNAMIC: %d \n", row.is_dynamic);
    printf("\t# IS FREE: %d \n", row.is_free);
    printf("\t# VAR_NAME: %s \n", row.var_name);
    printf("\t# LINE NUMBER: %d\n", row.line_number);
    printf("\n");
  }
}

void map2check_list_debug() {
  print_list_log(&list_map2check);
}


int map2check_non_det_int() {
  int non_det;
  klee_make_symbolic(&non_det,
		     sizeof(non_det),
		     "non_det_int");
  return non_det;
}

void map2check_malloc(void* ptr, int size) {
  if(!allocations_initialized) {
    allocations_map2check = new_memory_allocation();
    allocations_initialized = true;
  }

  mark_allocation_log(&allocations_map2check, (long) ptr);
}

int map2check_is_invalid_free(long ptr) {
  int i = list_map2check.size - 1;
  
  for(; i >= 0; i--) {
    long points_to = list_map2check.values[i]
      .memory_address_points_to;   

    if (points_to == ptr) {
      
      bool is_free = list_map2check
	.values[i].is_free;
      bool is_dynamic = list_map2check
	.values[i].is_dynamic;

      if(is_free || (!is_dynamic)) {
	return true;
      }
      else {
	return false;
      }
    } 
  }

  return true;
}

void map2check_ERROR() {
  map2check_list_debug();
  free_list_log(&list_map2check);
  klee_assert(0); 
}


void map2check_free( const char* name, void* ptr, unsigned scope, unsigned line,  const char* function_name) {
  if(!allocations_initialized) {
    allocations_map2check = new_memory_allocation();
    allocations_initialized = true;
  }

  long* addr = (long*) ptr;  

  if(list_initialized == false) {
    list_map2check = new_list_log();
    list_initialized = true;
  }

  LIST_LOG_ROW row = new_list_row((long) ptr,
				  (long) *addr,
				  scope,
				  false,
				  true,
				  line,
				  name);
  bool error = map2check_is_invalid_free((long) *addr);
  mark_deallocation_log(&allocations_map2check, (long) *addr);
  mark_map_log(&list_map2check, &row);

  if(error) {
    printf("VERIFICATION FAILED\n\n");
    printf("FALSE-FREE: Operand of free must have zero pointer offset\n");
    printf("Line %d in function %s\n\n", line, function_name);    
    printf("FAILED\n");
    map2check_ERROR();    
  }

}


void map2check_target_function(const char* func_name, int scope, int line) {
  printf("ERROR on Function %s :: SCOPE: %d :: LINE: %d\n", func_name, scope, line);
  map2check_ERROR();  
}
