#include "Map2CheckFunctions.h"
#include <stdio.h>
#include <stdlib.h>

int Map2CheckCurrentStep;

extern void __assert_fail(const char* assertion, const char* file,
                          unsigned int line, const char* function);

MAP2CHECK_CONTAINER list_log;
MAP2CHECK_CONTAINER klee_log;
MAP2CHECK_CONTAINER allocation_log;
MAP2CHECK_CONTAINER heap_log;
MAP2CHECK_CONTAINER trackbb_log;

Bool sv_comp = FALSE;
Bool gotError = FALSE;

Bool ERROR_DEREF = FALSE;
void map2check_init(int isSvComp) {
  Map2CheckCurrentStep = 0;
  list_log = new_container(LIST_LOG_CONTAINER);
  klee_log = new_container(KLEE_LOG_CONTAINER);
  allocation_log = new_container(ALLOCATION_LOG_CONTAINER);
  heap_log = new_container(HEAP_LOG_CONTAINER);
  trackbb_log = new_container(TRACKBB_LOG_CONTAINER);

  if (isSvComp) {
    sv_comp = TRUE;
  }
  write_property(UNKNOWN, 0, "");

  // map2check_alloca("NULL",0,1,1,1,1);
}

void map2check_track_bb(unsigned line, const char* function_name) {
  if (!(is_in_tracked(line, &trackbb_log))) {
    TRACK_BB_ROW* row = malloc(sizeof(TRACK_BB_ROW));
    *row = trackbb_new_row(line, function_name);
    append_element(&trackbb_log, row);
  }
}

void map2check_nondet_int(unsigned line, unsigned scope, int value,
                          const char* function_name) {
  /* int* result = (int*) malloc(sizeof(int)); */
  /* *result = value; */
  KLEE_CALL kleeCall = new_klee_call(INTEGER, line, scope, value, function_name,
                                     Map2CheckCurrentStep);
  Map2CheckCurrentStep++;

  KLEE_CALL* row = malloc(sizeof(KLEE_CALL));
  *row = kleeCall;

  append_element(&klee_log, row);
  /* klee_log_to_file(klee_log); */
}

void map2check_nondet_unsigned(unsigned line, unsigned scope, unsigned value,
                               const char* function_name) {
  KLEE_CALL kleeCall = new_klee_call(UNSIGNED, line, scope, value,
                                     function_name, Map2CheckCurrentStep);
  Map2CheckCurrentStep++;

  KLEE_CALL* row = malloc(sizeof(KLEE_CALL));
  *row = kleeCall;

  append_element(&klee_log, row);
  /* klee_log_to_file(klee_log); */
}

void map2check_nondet_char(unsigned line, unsigned scope, int value,
                           const char* function_name) {
  /* char* result = (char*) malloc(sizeof(char)); */
  /* *result = (char) value; */
  KLEE_CALL kleeCall = new_klee_call(CHAR, line, scope, value, function_name,
                                     Map2CheckCurrentStep);
  Map2CheckCurrentStep++;

  KLEE_CALL* row = malloc(sizeof(KLEE_CALL));
  *row = kleeCall;

  append_element(&klee_log, row);
  /* klee_log_to_file(klee_log); */
}

void map2check_nondet_pointer(unsigned line, unsigned scope, int value,
                              const char* function_name) {
  /* char* result = (char*) malloc(sizeof(char)); */
  /* *result = (char) value; */
  KLEE_CALL kleeCall = new_klee_call(POINTER, line, scope, value, function_name,
                                     Map2CheckCurrentStep);
  Map2CheckCurrentStep++;

  KLEE_CALL* row = malloc(sizeof(KLEE_CALL));
  *row = kleeCall;

  append_element(&klee_log, row);
  /* klee_log_to_file(klee_log); */
}

void map2check_nondet_ushort(unsigned line, unsigned scope, int value,
                             const char* function_name) {
  /* char* result = (char*) malloc(sizeof(char)); */
  /* *result = (char) value; */
  KLEE_CALL kleeCall = new_klee_call(USHORT, line, scope, value, function_name,
                                     Map2CheckCurrentStep);
  Map2CheckCurrentStep++;

  KLEE_CALL* row = malloc(sizeof(KLEE_CALL));
  *row = kleeCall;

  append_element(&klee_log, row);
  /* klee_log_to_file(klee_log); */
}

void map2check_nondet_long(unsigned line, unsigned scope, int value,
                           const char* function_name) {
  /* char* result = (char*) malloc(sizeof(char)); */
  /* *result = (char) value; */
  KLEE_CALL kleeCall = new_klee_call(LONG, line, scope, value, function_name,
                                     Map2CheckCurrentStep);
  Map2CheckCurrentStep++;

  KLEE_CALL* row = malloc(sizeof(KLEE_CALL));
  *row = kleeCall;

  append_element(&klee_log, row);
  /* klee_log_to_file(klee_log); */
}

void map2check_function(const char* name, void* ptr) {
  MEMORY_HEAP_ROW* row = malloc(sizeof(MEMORY_HEAP_ROW));
  *row = new_heap_row(1, 1, ptr, 1, 1, name);
  append_element(&heap_log, row);
}

const char* erro = "asd";
void map2check_load(void* ptr, int size) {
  if (!is_valid_heap_address(&heap_log, ptr, size)) {
    if (!is_valid_allocation_address(&allocation_log, ptr, size)) {
      // if(is_deref_error((long) ptr, &list_log)) {
      // printf("Is deref\n");
      ERROR_DEREF = TRUE;
      //}
    }
  }

  // map2check_check_deref();
}

void map2check_check_deref(int line, const char* function_name) {
  if (ERROR_DEREF) {
    write_property(FALSE_DEREF, line, function_name);

    map2check_error();
  }
}

void map2check_non_static_alloca(const char* name, void* ptr, int size,
                                 int size_of_primitive, int line, int scope) {
  map2check_alloca(name, ptr, size * size_of_primitive, size_of_primitive, line,
                   scope);
}

void map2check_alloca(const char* name, void* ptr, int size,
                      int size_of_primitive, int line, int scope) {
  MEMORY_HEAP_ROW* row = malloc(sizeof(MEMORY_HEAP_ROW));
  *row = new_heap_row(line, scope, ptr, size, size_of_primitive, name);
  append_element(&heap_log, row);
}

void map2check_add_store_pointer(void* var, void* value, unsigned scope,
                                 const char* name, int line,
                                 const char* function_name) {
  enum MemoryAddressStatus status =
      check_address_allocation_log(&allocation_log, (long)value);
  Bool isDynamic;
  Bool isFree;

  // Since we check in AllocationLog we can only known if the address is STATIC,
  // FREE or DYNAMIC
  switch (status) {
    case STATIC:
      isDynamic = FALSE;
      isFree = FALSE;
      break;
    case FREE:
      isDynamic = FALSE;
      isFree = TRUE;
      break;
    case DYNAMIC:
      isDynamic = TRUE;
      isFree = FALSE;
      break;
    default:
      isDynamic = FALSE;
      isFree = FALSE;
  }
  int i = list_log.size - 1;
  Bool isRedundant = FALSE;
  for (; i >= 0; i--) {
    LIST_LOG_ROW* iRow = (LIST_LOG_ROW*)get_element_at(i, list_log);
    if (iRow->memory_address == ((long)var)) {
      if ((iRow->memory_address_points_to == ((long)value)) &&
          (iRow->is_free == isFree) && (iRow->is_dynamic == isDynamic)) {
        isRedundant = TRUE;
      }
      break;
    }
  }

  if (!isRedundant) {
    LIST_LOG_ROW* row = malloc(sizeof(LIST_LOG_ROW));
    *row = new_list_row((long)var, (long)value, scope, isDynamic, isFree, line,
                        name, function_name, Map2CheckCurrentStep);
    Map2CheckCurrentStep++;

    append_element(&list_log, row);
    update_reference_list_log((long)value, status, line);
  }
}

int map2check_non_det_int() {
  int non_det;
  scanf("%i", &non_det);

  return non_det;
}

unsigned int map2check_non_det_uint() {
  unsigned int non_det;
  scanf("%u", &non_det);
  return non_det;
}

char map2check_non_det_char() {
  char non_det;
  scanf("%c", &non_det);
  return non_det;
}

unsigned short map2check_non_det_ushort() {
  unsigned short non_det;
  scanf("%hu", &non_det);
  return non_det;
}

void* map2check_non_det_pointer() {
  long non_det;
  scanf("%ld", &non_det);
  return (void*)non_det;
}
/*
void map2check_assume(int expr) {
  klee_assume(expr);
}
*/
long map2check_non_det_long() {
  long non_det;
  scanf("%ld", &non_det);
  return non_det;
}

void __VERIFIER_error() {
  if (!valid_allocation_log(&allocation_log)) {
    write_property(FALSE_MEMTRACK, 0, "");
  } else {
    write_property(NONE, 0, "");
  }
  map2check_exit();
  exit(0);
}

void map2check_target_function(const char* func_name, int scope, int line) {
  write_property(TARGET_REACHED, line, func_name);
  map2check_error();
}

void map2check_free_resolved_address(void* ptr, unsigned line,
                                     const char* function_name,
                                     short int isNullValid) {
  if ((ptr == NULL) && (isNullValid)) {
    return;
  }

  Bool error = is_invalid_free((long)ptr, &list_log);

  if (error) {
    write_property(FALSE_FREE, line, function_name);
    map2check_error();
  }
  MEMORY_ALLOCATIONS_ROW* row = find_row_with_address(&allocation_log, ptr);
  row->size = 0;
  row->is_free = TRUE;

  update_reference_list_log((long)ptr, FREE, line);
}

void map2check_posix(void* ptr, int size) {
  void** temp = (void**)ptr;
  void* addr = *temp;
  map2check_malloc(addr, size);
  map2check_add_store_pointer((void*)temp, addr, 1, "coisao", 1, "cocoiso");
}

void map2check_malloc(void* ptr, int size) {
  int addr = (int)ptr;
  //    printf("%d\n", addr);

  if (sv_comp && (addr == 0)) {
    //        write_property(UNKNOWN, 0, "");
    write_property_unknown();
    map2check_error();
  }

  MEMORY_ALLOCATIONS_ROW* row = find_row_with_address(&allocation_log, ptr);
  if (row != NULL) {
    row->size = size;
    row->is_free = FALSE;
  } else {
    row = malloc(sizeof(MEMORY_ALLOCATIONS_ROW));
    *row = new_memory_row((long)ptr, FALSE);
    row->size = size;
    append_element(&allocation_log, row);
  }
}

void map2check_calloc(void* ptr, int quantity, int size) {
  map2check_malloc(ptr, quantity * size);
}

void map2check_free(const char* name, void* ptr, unsigned scope, unsigned line,
                    const char* function_name) {
  long* addr = (long*)ptr;
  void* points_to = (void*)*addr;
  if (points_to == NULL) {
    return;
  }
  LIST_LOG_ROW* listRow = malloc(sizeof(LIST_LOG_ROW));
  *listRow = new_list_row((long)ptr, (long)*addr, scope, FALSE, TRUE, line,
                          name, function_name, Map2CheckCurrentStep);

  Map2CheckCurrentStep++;
  Bool error = is_invalid_free((long)*addr, &list_log);

  append_element(&list_log, listRow);

  if (error) {
    write_property(FALSE_FREE, line, function_name);
    map2check_error();
  }

  MEMORY_ALLOCATIONS_ROW* row =
      find_row_with_address(&allocation_log, (void*)*addr);
  row->size = 0;
  row->is_free = TRUE;

  //    MEMORY_ALLOCATIONS_ROW* row = malloc(sizeof(MEMORY_ALLOCATIONS_ROW));
  //     *row =  new_memory_row((long) *addr, TRUE);

  //    append_element(&allocation_log, row);

  //    append_element(&allocation_log, row);

  update_reference_list_log((long)*addr, FREE, line);
}

void map2check_success() {
  if (!valid_allocation_log(&allocation_log)) {
    write_property(FALSE_MEMTRACK, 0, "");
    map2check_error();

  } else {
    write_property(NONE, 0, "");
    map2check_exit();
  }
}

void map2check_error() {
  gotError = TRUE;
  map2check_exit();
  // klee_assert(0);
  //
}

void map2check_exit() {
  //    printf("\n\n\n\n\n");
  klee_log_to_file(klee_log);
  free_container(&klee_log);

  list_log_to_file(&list_log);
  free_container(&list_log);

  heap_log_to_file(&heap_log);
  free_container(&heap_log);

  trackbb_log_to_file(&trackbb_log);
  free_container(&trackbb_log);

  allocation_log_to_file(&allocation_log);
  valid_allocation_log(&allocation_log);
  free_container(&allocation_log);
}

void update_reference_list_log(long address, enum MemoryAddressStatus status,
                               unsigned line) {
  int i = 0;
  int currentSize = list_log.size;
  for (; i < currentSize; i++) {
    LIST_LOG_ROW* iRow = (LIST_LOG_ROW*)get_element_at(i, list_log);
    if (!iRow) {
    }
    long currentAddress = iRow->memory_address_points_to;
    long currentVarAddress = iRow->memory_address;

    if ((currentAddress == address) &&
        (status != get_type_from_list_log_row(iRow))) {
      int j = currentSize - 1;

      for (; i <= j; j--) {
        LIST_LOG_ROW* jRow = (LIST_LOG_ROW*)get_element_at(j, list_log);
        if (!jRow) {
        }
        long otherAddress = jRow->memory_address_points_to;
        long otherVarAddress = jRow->memory_address;
        int sameVar = otherVarAddress == currentVarAddress;

        if ((otherAddress == address) && (sameVar)) {
          enum MemoryAddressStatus otherStatus =
              get_type_from_list_log_row(jRow);
          if (otherStatus != status) {
            Bool isDynamic;
            Bool isFree;

            switch (status) {
              case STATIC:
                isDynamic = FALSE;
                isFree = FALSE;
                break;
              case FREE:
                isDynamic = FALSE;
                isFree = TRUE;
                break;
              case DYNAMIC:
                isDynamic = TRUE;
                isFree = FALSE;
                break;
            }
            LIST_LOG_ROW* row = malloc(sizeof(LIST_LOG_ROW));
            *row = new_list_row(jRow->memory_address,
                                jRow->memory_address_points_to, jRow->scope,
                                isDynamic, isFree, line, jRow->var_name,
                                jRow->function_name, Map2CheckCurrentStep);
            Map2CheckCurrentStep++;
            append_element(&list_log, row);
          }
          break;
        }
      }
    }
  }
}
