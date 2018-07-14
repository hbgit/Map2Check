#include "AllocationLog.h"
#include <stdio.h>
#include <stdlib.h>

Bool mark_allocation_log(MAP2CHECK_CONTAINER* allocation_log, long address) {
  if (allocation_log->type != ALLOCATION_LOG_CONTAINER) {
    return FALSE;
  }
  MEMORY_ALLOCATIONS_ROW row = new_memory_row(address, FALSE);

  return append_element(allocation_log, &row);
}

Bool mark_deallocation_log(MAP2CHECK_CONTAINER* allocation_log, long address) {
  if (allocation_log->type != ALLOCATION_LOG_CONTAINER) {
    return FALSE;
  }

  MEMORY_ALLOCATIONS_ROW row = new_memory_row(address, TRUE);

  return append_element(allocation_log, &row);
}

enum MemoryAddressStatus check_address_allocation_log(
    MAP2CHECK_CONTAINER* allocation_log, long address) {
  int i = allocation_log->size - 1;

  for (; i >= 0; i--) {
    MEMORY_ALLOCATIONS_ROW* row =
        (MEMORY_ALLOCATIONS_ROW*)get_element_at(i, *allocation_log);
    if (row->addr == address) {
      if (row->is_free == TRUE) {
        return FREE;
      } else {
        return DYNAMIC;
      }
    }
  }

  return STATIC;
}

MEMORY_ALLOCATIONS_ROW new_memory_row(long address, Bool is_free) {
  MEMORY_ALLOCATIONS_ROW row;
  row.addr = address;
  row.is_free = is_free;
  return row;
}

/* The ideia of this function is to check if all address allocated
** were released at the end of the program, to check this, we iterate
** over all elements starting from bottom of allocation log, if the
** address of the current item is not Free, we iterate from the top
** if we find that the address was released, then we go on
** if not we return FALSE. */
Bool valid_allocation_log(MAP2CHECK_CONTAINER* allocation_log) {
  Bool MemTrackError = FALSE;
  int i = 0;
  // int size = allocation_log->size;
  for (; i < allocation_log->size; i++) {
    MEMORY_ALLOCATIONS_ROW* iRow =
        (MEMORY_ALLOCATIONS_ROW*)get_element_at(i, *allocation_log);
    if (!iRow->is_free) {
      long addr = iRow->addr;
      Bool foundReleased = FALSE;
      int j = allocation_log->size - 1;
      // FIXME: It should be from top
      for (; j > i; j--) {
        MEMORY_ALLOCATIONS_ROW* jRow =
            (MEMORY_ALLOCATIONS_ROW*)get_element_at(j, *allocation_log);
        if (jRow->addr == addr && jRow->is_free) {
          foundReleased = TRUE;
          break;
        }
      }

      if (!foundReleased) {
        free((void*)addr);
        MEMORY_ALLOCATIONS_ROW* row = malloc(sizeof(MEMORY_ALLOCATIONS_ROW));
        *row = new_memory_row(addr, TRUE);
        append_element(allocation_log, row);
        MemTrackError = TRUE;
      }
    }
  }
  return !MemTrackError;
}

MEMORY_ALLOCATIONS_ROW* find_row_with_address(
    MAP2CHECK_CONTAINER* allocation_log, void* ptr) {
  int i = allocation_log->size - 1;

  unsigned addressToCheck = (unsigned)ptr;
  for (; i >= 0; i--) {
    MEMORY_ALLOCATIONS_ROW* iRow =
        (MEMORY_ALLOCATIONS_ROW*)get_element_at(i, *allocation_log);

    unsigned addr = (unsigned)iRow->addr;
    if (addressToCheck == addr) {
      // printf("Got here2: %p\n", ptr);
      return iRow;
    }
  }
  return NULL;
}

/* Very simple ideia, we iterate over all elements of the allocation log,
 * beggining from top, if the address that we are looking for is in the range of
 * the element address memory space and the element is not free, then it's TRUE
 * otherwise FALSE and we set last_address to the address of last memory address
 * of the current heap space i.e. a int on space 0x10 on a 32bit would set the
 * var to 0x13 (if the int has 4 bytes)
 */
Bool is_valid_allocation_address(MAP2CHECK_CONTAINER* allocation_log,
                                 void* address, int size_to_destiny) {
  /*if(!address){
    return TRUE;
    }*/
  int i = allocation_log->size - 1;
  unsigned addressToCheck = (unsigned)address;
  for (; i >= 0; i--) {
    MEMORY_ALLOCATIONS_ROW* iRow =
        (MEMORY_ALLOCATIONS_ROW*)get_element_at(i, *allocation_log);
    unsigned addressBottom = iRow->addr;
    unsigned addressTop = addressBottom + iRow->size - size_to_destiny + 1;
    //*last_address = addressTop;

    if ((addressBottom <= addressToCheck) && (addressToCheck < addressTop)) {
      if (iRow->is_free) {
        return FALSE;
      }
      //*last_address = addressTop;
      return TRUE;
    }
  }

  //*last_address = 0;
  return FALSE;
}

void release_unreleased_addresses(MAP2CHECK_CONTAINER* list) {
  int i = 0;
  for (; i < list->size; i++) {
    MEMORY_ALLOCATIONS_ROW* row =
        (MEMORY_ALLOCATIONS_ROW*)get_element_at(i, *list);
    if (!row->is_free) {
      free((void*)row->addr);
    }
  }
}

void allocation_log_to_file(MAP2CHECK_CONTAINER* list) {
  FILE* output = fopen("allocation_log.csv", "w");
  // fprintf(output, "id;memory address;points to;scope;is free;is
  // dynamic;function name\n");
  int i = 0;
  for (; i < list->size; i++) {
    MEMORY_ALLOCATIONS_ROW* row =
        (MEMORY_ALLOCATIONS_ROW*)get_element_at(i, *list);
    fprintf(output, "%p;", (void*)row->addr);
    fprintf(output, "%u;", row->size);
    fprintf(output, "%d\n", row->is_free);
  }
  fclose(output);
}
