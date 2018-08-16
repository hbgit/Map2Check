// This file describes all methods that are used for MemtrackAnalysis
// DEFINITIONS
#include "../header/Map2CheckTypes.h"
/**
 * @brief Checks if address to be loaded is valid
 * @param ptr    Pointer to where value will be loaded
 * @param size   Size of the type to be loaded
 */
void map2check_load(void *ptr, int size);
/**
 * Tracks address that are resolved during free (this function is to be used for
 * instrumentation)
 * @param ptr         Address to be released
 * @param line        Line where store operation was called
 * @param func_name   Name of the function where the operation occured.
 * @param isNullValid If value is not 0, ignores if ptr points to NULL
 */
void map2check_free_resolved_address(void *ptr, unsigned line,
                                     const char *function_name,
                                     short int isNullValid);
/**
 * Updates all variables that points to the address with new status
 * @param address  Address to be updates
 * @param status  Current memory status
 * @param line  Line where operation happened
 */
void update_reference_list_log(long address, enum MemoryAddressStatus status,
                               unsigned line);
/**
 * Track pointer store operations (this function is to be used for
 * instrumentation)
 * @param var   Address of current pointer
 * @param value New address to where pointer points to
 * @param scope Number of the scope
 * @param name  Name of the pointer
 * @param line  Line where store operation was called
 * @param function_name  Name of the function where the operation occured.
 */
void map2check_add_store_pointer(void *var, void *value, unsigned scope,
                                 const char *name, int line,
                                 const char *function_name);
/**
 * @brief Checks if current state of program is a deref error
 * @param line           Line where deref error occurred
 * @param function_name  Name of the function where operation occurred
 */
void map2check_check_deref(int line, const char *function_name);
/**
 * Tracks address that are resolved during free (this function is to be used for
 * instrumentation)
 * @param ptr         Address allocated
 * @param size        Size of the allocated addres
 */
void map2check_malloc(void *ptr, int size);
/**
 * Tracks address that are resolved during free (this function is to be used for
 * instrumentation)
 * @param ptr         Address allocated
 * @param size        Size of the allocated addres
 */
void map2check_posix(void *ptr, int size);
/**
 * Tracks address that are resolved during free (this function is to be used for
 * instrumentation)
 * @param ptr         Address allocated
 * @param size        Size of the allocated addres
 */
void map2check_calloc(void *ptr, int quantity, int size);
/**
 * @brief Tracks addresses from memory heap
 * @param name               Name of the variable allocated
 * @param ptr                Memory address allocated
 * @param size               Size of the allocated address
 * @param size_of_primitive  Size of the primitive on allocated address
 */
void map2check_alloca(const char *name, void *ptr, int size,
                      int size_of_primitive, int line, int scope);

// TODO: document method
void map2check_non_static_alloca(const char *name, void *ptr, int size,
                                 int size_of_primitive, int line, int scope);

/**
 * @brief Tracks addresses from memory heap
 * @param name Name of the variable allocated
 * @param ptr  Memory address allocated
 */
void map2check_function(const char *name, void *ptr);

/**
 * Tracks address that are freeed (this function is to be used for
 * instrumentation)
 * @param name        Name of the variable
 * @param ptr         Address to be released
 * @param scope       Number of the scope
 * @param size        Size of the allocated addres
 * @param line        Line where store operation was called
 * @param func_name   Name of the function where the operation occured.
 */
void map2check_free(const char *name, void *ptr, unsigned scope, unsigned line,
                    const char *function_name);

// IMPLEMENTATION
#include "../header/AllocationLog.h"
#include "../header/Container.h"
#include "../header/HeapLog.h"
#include "../header/ListLog.h"
#include "../header/Map2CheckFunctions.h"
#include "../header/PropertyGenerator.h"
#include <stdlib.h>

MAP2CHECK_CONTAINER heap_log;
MAP2CHECK_CONTAINER allocation_log;
MAP2CHECK_CONTAINER list_log;
Bool ERROR_DEREF;

void analysis_init() {
  heap_log = new_container(HEAP_LOG_CONTAINER);
  allocation_log = new_container(ALLOCATION_LOG_CONTAINER);
  list_log = new_container(LIST_LOG_CONTAINER);
  ERROR_DEREF = FALSE;
}

void analysis_destroy() {
  free_container(&heap_log);
  free_container(&list_log);
  free_container(&allocation_log);
}

Bool analysis_is_program_correct() {
  if (!valid_allocation_log(&allocation_log)) {
    write_property(FALSE_MEMTRACK, 0, "");
    return FALSE;
  }
  return TRUE;
}

void analysis_generate_aux_witness_files() {
  heap_log_to_file(&heap_log);
  allocation_log_to_file(&allocation_log);
  list_log_to_file(&list_log);
}

// TODO: FIX THIS, not working for static array
#include <stdio.h>
void map2check_load(void *ptr, int size) {
  if (!is_valid_heap_address(&heap_log, ptr, size)) {
    printf("Got here with addr %p, and size %d\n", ptr, size);
    if (!is_valid_allocation_address(&allocation_log, ptr, size)) {

      ERROR_DEREF = TRUE;
    }
  }
}

void map2check_free_resolved_address(void *ptr, unsigned line,
                                     const char *function_name,
                                     short int isNullValid) {
  if ((ptr == NULL) && (isNullValid)) {
    return;
  }

  Bool error = is_invalid_free((long)ptr, &list_log);

  if (error) {
    write_property(FALSE_FREE, line, function_name);
    map2check_error();
  }
  MEMORY_ALLOCATIONS_ROW *row = find_row_with_address(&allocation_log, ptr);
  row->size = 0;
  row->is_free = TRUE;

  update_reference_list_log((long)ptr, FREE, line);
}

void update_reference_list_log(long address, enum MemoryAddressStatus status,
                               unsigned line) {
  int i = 0;
  int currentSize = list_log.size;
  for (; i < currentSize; i++) {
    LIST_LOG_ROW *iRow = (LIST_LOG_ROW *)get_element_at(i, list_log);
    if (!iRow) {
    }
    long currentAddress = iRow->memory_address_points_to;
    long currentVarAddress = iRow->memory_address;

    if ((currentAddress == address) &&
        (status != get_type_from_list_log_row(iRow))) {
      int j = currentSize - 1;

      for (; i <= j; j--) {
        LIST_LOG_ROW *jRow = (LIST_LOG_ROW *)get_element_at(j, list_log);
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
            LIST_LOG_ROW *row = (LIST_LOG_ROW *)malloc(sizeof(LIST_LOG_ROW));
            *row = new_list_row(
                jRow->memory_address, jRow->memory_address_points_to,
                jRow->scope, isDynamic, isFree, line, jRow->var_name,
                jRow->function_name, map2check_get_current_step());
            map2check_next_current_step();
            append_element(&list_log, row);
          }
          break;
        }
      }
    }
  }
}

void map2check_add_store_pointer(void *var, void *value, unsigned scope,
                                 const char *name, int line,
                                 const char *function_name) {
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
    LIST_LOG_ROW *iRow = (LIST_LOG_ROW *)get_element_at(i, list_log);
    if (iRow->memory_address == ((long)var)) {
      if ((iRow->memory_address_points_to == ((long)value)) &&
          (iRow->is_free == isFree) && (iRow->is_dynamic == isDynamic)) {
        isRedundant = TRUE;
      }
      break;
    }
  }

  if (!isRedundant) {
    LIST_LOG_ROW *row = (LIST_LOG_ROW *)malloc(sizeof(LIST_LOG_ROW));
    *row = new_list_row((long)var, (long)value, scope, isDynamic, isFree, line,
                        name, function_name, map2check_get_current_step());
    map2check_next_current_step();

    append_element(&list_log, row);
    update_reference_list_log((long)value, status, line);
  }
}

void map2check_check_deref(int line, const char *function_name) {
  if (ERROR_DEREF) {
    write_property(FALSE_DEREF, line, function_name);
    map2check_error();
  }
}

void map2check_malloc(void *ptr, int size) {
  int addr = (long)ptr;
  if (addr == 0) {
    //        write_property(UNKNOWN, 0, "");
    write_property_unknown();
    map2check_error();
  }

  MEMORY_ALLOCATIONS_ROW *row = find_row_with_address(&allocation_log, ptr);
  if (row != NULL) {
    row->size = size;
    row->is_free = FALSE;
  } else {
    row = (MEMORY_ALLOCATIONS_ROW *)malloc(sizeof(MEMORY_ALLOCATIONS_ROW));
    *row = new_memory_row((long)ptr, FALSE);
    row->size = size;
    append_element(&allocation_log, row);
  }
}

// TODO: Should we remove this?
void map2check_posix(void *ptr, int size) {
  void **temp = (void **)ptr;
  void *addr = *temp;
  map2check_malloc(addr, size);
  map2check_add_store_pointer((void *)temp, addr, 1, "coisao", 1, "cocoiso");
}

void map2check_calloc(void *ptr, int quantity, int size) {
  map2check_malloc(ptr, quantity * size);
}

void map2check_free(const char *name, void *ptr, unsigned scope, unsigned line,
                    const char *function_name) {
  long *addr = (long *)ptr;
  void *points_to = (void *)*addr;
  if (points_to == NULL) {
    return;
  }
  LIST_LOG_ROW *listRow = (LIST_LOG_ROW *)malloc(sizeof(LIST_LOG_ROW));
  *listRow = new_list_row((long)ptr, (long)*addr, scope, FALSE, TRUE, line,
                          name, function_name, map2check_get_current_step());

  map2check_next_current_step();
  Bool error = is_invalid_free((long)*addr, &list_log);

  append_element(&list_log, listRow);

  if (error) {
    write_property(FALSE_FREE, line, function_name);
    map2check_error();
  }

  MEMORY_ALLOCATIONS_ROW *row =
      find_row_with_address(&allocation_log, (void *)*addr);
  row->size = 0;
  row->is_free = TRUE;

  update_reference_list_log((long)*addr, FREE, line);
}

void map2check_non_static_alloca(const char *name, void *ptr, int size,
                                 int size_of_primitive, int line, int scope) {
  map2check_alloca(name, ptr, size * size_of_primitive, size_of_primitive, line,
                   scope);
}

void map2check_alloca(const char *name, void *ptr, int size,
                      int size_of_primitive, int line, int scope) {
  MEMORY_HEAP_ROW *row = (MEMORY_HEAP_ROW *)malloc(sizeof(MEMORY_HEAP_ROW));
  *row = new_heap_row(line, scope, ptr, size, size_of_primitive, name);
  append_element(&heap_log, row);
}

void map2check_function(const char *name, void *ptr) {
  MEMORY_HEAP_ROW *row = (MEMORY_HEAP_ROW *)malloc(sizeof(MEMORY_HEAP_ROW));
  *row = new_heap_row(1, 1, ptr, 1, 1, name);
  append_element(&heap_log, row);
}
