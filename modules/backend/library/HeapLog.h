/**********************************************************************
 * This file contains all methods that are used for the HeapLog      *
 * which is a structure that contains all heap memory operatio         *
 ***********************************************************************/
#include "Map2CheckTypes.h"
#include "Container.h"

#ifndef HEAPLOG_H
#define HEAPLOG_H

/**
 * Mark element as heap address to a MAP2CHECK_CONTAINER
 * @param  heap_log       Pointer to the MAP2CHECK_CONTAINER
 * @param  row            Row of the MEMORY_HEAP_ROW to be added on the container
 * @return                Returns success of operation
 */
Bool mark_heap_log(MAP2CHECK_CONTAINER* heap_log, MEMORY_HEAP_ROW row);

/**
 * Iterates over a Heap Log to check if a address is pointing to a valid heap address
 * @param  heap_log Pointer to the MAP2CHECK_CONTAINER
 * @param  address        Heap address
 * @param  last_address   Extra variable to get the last address of the memory space
 * @return                Returns if the address is valid
 */
Bool is_valid_heap_address(MAP2CHECK_CONTAINER* heap_log, void* address, int size_to_load);

/**
 * This method initializes a MEMORY_HEAP_ROW with the defined parameters
 * @param  line                 Line where operation occurred
 * @param  scope                Scope where operation occurred
 * @param  address              Memory Address to be logged
 * @param  size                 Size of the heap address
 * @param  size_of_primitive    Size of the primitive on the heap address
 * @param  function_name        Name of the function where operation took place
 * @return                      Returns the initialized MEMORY_HEAP_ROW
 */
MEMORY_HEAP_ROW new_heap_row(int line, int scope, void* address, int size, int size_of_primitive, const char* function_name);

/**
 * Write elements from HEAP_LOG to a FILE
 * @param row Pointer to the HEAP_LOG to be writed
 */
void heap_log_to_file(MAP2CHECK_CONTAINER* list);



#endif
