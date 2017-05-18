#include "AllocationLog.h"

Bool mark_allocation_log(MAP2CHECK_CONTAINER* allocation_log, long address) {
    if(allocation_log->type != ALLOCATION_LOG_CONTAINER) {
        return FALSE;
    }
    MEMORY_ALLOCATIONS_ROW row = new_memory_row(address, FALSE);

    return append_element(allocation_log, &row);
}

Bool mark_deallocation_log(MAP2CHECK_CONTAINER* allocation_log, long address) {
    if(allocation_log->type != ALLOCATION_LOG_CONTAINER) {
        return FALSE;
    }

    MEMORY_ALLOCATIONS_ROW row = new_memory_row(address, TRUE);

    return append_element(allocation_log, &row);
}

enum MemoryAddressStatus check_address_allocation_log(MAP2CHECK_CONTAINER* allocation_log, long address) {
    int i = allocation_log->size - 1;

    for(; i >= 0; i--) {
        //printf("Testing check address\n");
        MEMORY_ALLOCATIONS_ROW* row = (MEMORY_ALLOCATIONS_ROW*) get_element_at(i, *allocation_log); 
        if(row->addr == address) {
         if(row->is_free == TRUE) {
            return FREE;
        }
         else {
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
  printf("Checking for MemTrack\n");
  int i = 0;
  int size = allocation_log->size;
  for(; i < size; i++) {
      MEMORY_ALLOCATIONS_ROW* iRow = (MEMORY_ALLOCATIONS_ROW*) get_element_at(i, *allocation_log); 
    if(!iRow->is_free) {
      long addr = iRow->addr;
      Bool foundReleased = FALSE;
      int j = size - 1;
      //FIXME: It should be from top
      for (; j > i; j--) {
          MEMORY_ALLOCATIONS_ROW* jRow = (MEMORY_ALLOCATIONS_ROW*) get_element_at(j, *allocation_log); 
        if (jRow->addr == addr && jRow->is_free ) {          
            foundReleased = TRUE;
        }
      }

      if(!foundReleased) {
        printf("Address not released: %p\n", addr);
        return FALSE;
      }
    }
  }
  return TRUE;
}

/* Very simple ideia, we iterate over all elements of the allocation log, beggining from
 * top, if the address that we are looking for is in the range of the element address memory
 * space and the element is not free, then it's TRUE otherwise FALSE
 * and we set last_address to the address of last memory address of the current heap space
 * i.e. a int on space 0x10 on a 32bit would set the var to 0x13 (if the int has 4 bytes)
 */
Bool is_valid_allocation_address(MAP2CHECK_CONTAINER* allocation_log, void* address, long* last_address) {
    printf("Checking for Dynamic Deref\n");
    int i = allocation_log->size - 1;
    long addressToCheck = (long) address;
    for(; i >= 0; i--) {
        MEMORY_ALLOCATIONS_ROW* iRow = (MEMORY_ALLOCATIONS_ROW*) get_element_at(i, *allocation_log);
        long addressBottom = iRow->addr;
        long addressTop = addressBottom + iRow->size;

        if((addressBottom <= addressToCheck) && (addressToCheck < addressTop)) {
            if(iRow->is_free){
                *last_address = 0;
                return FALSE;
            }
            *last_address = addressTop;
            return TRUE;
        }
    }

    *last_address = 0;
    return FALSE;
}
