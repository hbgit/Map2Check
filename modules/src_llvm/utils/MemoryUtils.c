#include <stdio.h>
// #include <stdlib.h>
#include <string.h>
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

KLEE_LOG klee_map2check;
bool klee_initialized = false;

unsigned current_step = 0;

void add_klee_call_to_log(KLEE_LOG* log, KLEE_CALL call) {
  log->size++;
  KLEE_CALL* new_values = (KLEE_CALL*) realloc (log->values, log->size * sizeof(KLEE_CALL));
  log->values = new_values;
  log->values[log->size-1] = call;
}

void print_klee_log(KLEE_LOG* log) {
  int i;
  printf("******* KLEE LOG ******* \n");
  for(i = 0; i < log->size; i++) {
    printf("* ID: %d\n", i);
    printf("\tLine: %d\n", log->values[i].line);
    printf("\tScope: %d\n", log->values[i].scope);
    printf("\tFunction: %s\n", log->values[i].function_name);
    printf("\tStep: %d\n", log->values[i].step_on_execution);

    // switch (log->values[i].type) {
    //   case INTEGER:
    //     // printf("\tValue: %d\n\n", *((int*)log->values[i].value));
    // }

  }
}

void klee_log_to_file(KLEE_LOG* log) {
  FILE* output = fopen("klee_log.csv", "w");
  // fprintf(output, "id;memory address;points to;scope;is free;is dynamic;function name\n");
  int i = 0;
  for(;i< log->size; i++) {
    fprintf(output,"%d;", i);
    fprintf(output,"%d;", log->values[i].line);
    fprintf(output,"%d;", log->values[i].scope);
    fprintf(output,"%s;", log->values[i].function_name);
    fprintf(output,"%d;", log->values[i].step_on_execution);

    switch (log->values[i].type) {
      case INTEGER:
        fprintf(output,"%d\n", (*(int*)log->values[i].value));
    }
  }
  fclose(output);
}

void free_klee_log(KLEE_LOG* log) {
  int i;
  for(i = 0; i < log->size; i++) {
    free(log->values[i].value);
  }
  free(log->values);
}

void map2check_klee_int(unsigned line, unsigned scope, int value, const char* function_name) {
  if (!klee_initialized) {
    klee_map2check.size = 0;
    klee_map2check.values = NULL;
    klee_initialized = true;
  }

  int coiso = value;
  // printf("RESULT = %d\n", value);
  int* result = (int*) malloc(sizeof(int));
  *result = value;
  KLEE_CALL kleeCall = new_klee_call(INTEGER, line, scope, result,function_name);
  add_klee_call_to_log(&klee_map2check, kleeCall);

}
KLEE_CALL new_klee_call(enum NONDET_TYPE type, unsigned line, unsigned scope, void* value, const char* function_name) {
  KLEE_CALL result;
  result.type = type;
  result.function_name = function_name;
  result.line = line;
  result.value = value;
  result.scope = scope;
  result.step_on_execution = current_step;
  current_step++;
  return result;
}


int map2check_non_det_int() {
  int non_det;
  klee_make_symbolic(&non_det,
		     sizeof(non_det),
		     "non_det_int");
  return non_det;
}

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
void map2check_add_store_pointer(void* var, void* value,unsigned scope, const char* name, int line,  const char* function_name) {
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
  LIST_LOG_ROW row = new_list_row((long) var,(long) value, scope, isDynamic, isFree, line, name, function_name);
  void* oldAddress = getOldReference(name, &list_map2check);
   mark_map_log(&list_map2check, &row);

  status = check_address_allocation_log(&allocations_map2check, (long) oldAddress);

  // If memory address is dynamic we should check if another var points to that addres, if not
  // it may be a deref  
  if (status == DYNAMIC) {
    // if(isDerefError((long) oldAddress, &list_map2check)) {
    //   FILE* output = fopen("map2check_property", "w");
    //   fprintf(output, "FALSE-DEREF\n");
    //   fprintf(output, "Line: %d\n", line);
    //   fprintf(output, "Function: %s\n", function_name);
    //   fclose(output);
    //   map2check_ERROR();
    // }
  }

 
}

bool isDerefError(long address, LIST_LOG* log) {
  int i = log->size - 1;
  for(;i >= 0 ; i--) {
    LIST_LOG_ROW* row = &log->values[i];
    if(row->memory_address_points_to == address) {
       printf("SEARCHING FOR PREVIOUS CALL WITH VAR %s",row->var_name );
       int j = log->size - 1;
       for(; j > i; j--) {
         LIST_LOG_ROW* row2 = &log->values[j];
         if (!strcmp(row->var_name, row2->var_name)) {
           printf("FOUND %ld",row2->memory_address_points_to );
           if(row2->memory_address_points_to != address) {
              return true;      
           }
           else {
             break;
           }
         }
       }
    }
  }

  
  return false;
}

void* getOldReference(const char* varName, LIST_LOG* log) {
  int i = log->size - 1;
  for(;i >= 0 ; i--) {
    LIST_LOG_ROW* row = &log->values[i];

    if (!strcmp(row->var_name, varName)) {
      return row->memory_address_points_to;
    }
  }

  //FIXME: Throw fatal
  return 0;
}

void map2check_pointer(void* x,unsigned scope, const char* name, int line){
  if(list_initialized == false) {
    list_map2check = new_list_log();
    list_initialized = true;
  }

  // TODO: Add current function name
  LIST_LOG_ROW row = new_list_row((long) x, 0, scope, true, false, line,name, "function_example_map2check");
  mark_map_log(&list_map2check, &row);
}

void list_log_to_file(LIST_LOG* list) {
  FILE* output = fopen("list_log.csv", "w");
  // fprintf(output, "id;memory address;points to;scope;is free;is dynamic;function name\n");
  int i = 0;
  for(;i< list->size; i++) {
    LIST_LOG_ROW*row = &list->values[i];
    fprintf(output, "%d;", row->id);
    fprintf(output, "%p;", row->memory_address);
    fprintf(output, "%p;", row->memory_address_points_to);
    fprintf(output, "%d;", row->scope);
    fprintf(output, "%d;", row->is_free);
    fprintf(output, "%d;", row->is_dynamic);
    fprintf(output, "%s;", row->var_name);
    fprintf(output, "%d;", row->line_number);
    fprintf(output, "%s;", row->function_name);
    fprintf(output, "%d\n", row->step_on_execution);
  }
  fclose(output);
}

LIST_LOG_ROW new_list_row (long memory_address, long memory_address_points_to,
			   unsigned scope, bool is_dynamic, bool is_free, unsigned line_number,
         const char* name, const char* function_name) {
  LIST_LOG_ROW row;
  row.id = 0;
  row.is_dynamic = is_dynamic;
  row.is_free = is_free;
  row.line_number = line_number;
  row.memory_address = memory_address;
  row.memory_address_points_to = memory_address_points_to;
  row.scope = scope;
  row.var_name = name;
  row.function_name = function_name;
  row.step_on_execution = current_step;
  current_step++;

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
    printf("\t# FUNCTION NAME: %s\n", row.function_name);
    printf("\n");
  }
}

void map2check_list_debug() {
  print_list_log(&list_map2check);
}

void map2check_malloc(void* ptr, int size) {
  if(!allocations_initialized) {
    allocations_map2check = new_memory_allocation();
    allocations_initialized = true;
  }

  mark_allocation_log(&allocations_map2check, (long) ptr);
}

// TODO: Fix function, if it does not found address, it should be a deref problem
int map2check_is_invalid_free(long ptr) {
  int i = list_map2check.size - 1;

  for(; i >= 0; i--) {
    long points_to = list_map2check.values[i].memory_address_points_to;

    if (points_to == ptr) {

      bool is_free = list_map2check.values[i].is_free;
      bool is_dynamic = list_map2check.values[i].is_dynamic;

      if(is_free || (!is_dynamic)) {
	      return true;
      }
      else {
	      return false;
      }
    }
  }

   //FIXME: Throw fatal
  return true;
}

void map2check_ERROR() {
  map2check_list_debug();
  list_log_to_file(&list_map2check);
  klee_log_to_file(&klee_map2check);
  // print_klee_log(&klee_map2check);

  free_list_log(&list_map2check);
  free_klee_log(&klee_map2check);
  klee_assert(0);
}

void map2check_success() {
  free_list_log(&list_map2check);
  free_klee_log(&klee_map2check);
  FILE* output = fopen("map2check_property", "w");
  fprintf(output, "NONE");
  fclose(output);
}


void map2check_free( const char* name, void* ptr, unsigned scope, const unsigned line,  const char* function_name) {
  if(!allocations_initialized) {
    allocations_map2check = new_memory_allocation();
    allocations_initialized = true;
  }

  long* addr = (long*) ptr;

  if(list_initialized == false) {
    list_map2check = new_list_log();
    list_initialized = true;
  }

  LIST_LOG_ROW row = new_list_row((long) ptr, (long) *addr, scope, false,
				  true, line, name, function_name);

  bool error = map2check_is_invalid_free((long) *addr);

  if(error) {
    printf("VERIFICATION FAILED\n\n");
    printf("FALSE-FREE: Operand of free must have zero pointer offset\n");
    printf("Line %d in function %s\n\n", line, function_name);
    printf("FAILED\n");
    FILE* output = fopen("map2check_property", "w");
    fprintf(output, "FALSE-FREE\n");
    fprintf(output, "Line: %d\n", line);
    fprintf(output, "Function: %s\n", function_name);
    fclose(output);
    map2check_ERROR();
  }

  mark_deallocation_log(&allocations_map2check, (long) *addr);
  mark_map_log(&list_map2check, &row);

}

void map2check_target_function(const char* func_name, int scope, int line) {
  printf("ERROR on Function %s :: SCOPE: %d :: LINE: %d\n", func_name, scope, line);
  FILE* output = fopen("map2check_property", "w");
  fprintf(output, "TARGET-REACHED\n");
  fprintf(output, "Line: %d\n", line);
  fprintf(output, "Function: %s\n", func_name);
  fclose(output);
  map2check_ERROR();
}
