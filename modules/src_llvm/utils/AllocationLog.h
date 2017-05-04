/**********************************************************************
* This file contains all methods that are used for the AllocationLog  *
* which is a structure that contains all memory operations            *
***********************************************************************/
#include "Map2CheckTypes.h"
#include "Container.h"

#ifndef AllocationLog_H
#define AllocationLog_H

/**
 * Mark element as not free to a MAP2CHECK_CONTAINER
 * @param  allocation_log Pointer to the MAP2CHECK_CONTAINER
 * @param  address        Memory Address to be added
 * @return                Returns success of operation
 */
Bool mark_allocation_log(MAP2CHECK_CONTAINER* allocation_log, long address);

/**
 * Mark element as free to a MAP2CHECK_CONTAINER
 * @param  allocation_log Pointer to the MAP2CHECK_CONTAINER
 * @param  address        Memory Address to be freed
 * @return                Returns success of operation
 */
Bool mark_deallocation_log(MAP2CHECK_CONTAINER* allocation_log, long address);

/**
 * Checks if an address exists in a MAP2CHECK_CONTAINER, it checks from the last added to the first
 * @param  allocation_log Pointer to the MAP2CHECK_CONTAINER
 * @param  address        Memory Address to be checked
 * @return                Returns the most recent status of the address
 */
enum MemoryAddressStatus check_address_allocation_log(MAP2CHECK_CONTAINER* allocation_log, long address);


// void print_allocation_log(MAP2CHECK_CONTAINER* allocation_log);

/**
 * This method initializes a MEMORY_ALLOCATIONS_ROW with the defined parameters
 * @param  address Memory Address to be logged
 * @param  is_free Is the *address* released?
 * @return         Returns the initialized MEMORY_ALLOCATIONS_ROW
 */
MEMORY_ALLOCATIONS_ROW new_memory_row(long address, Bool is_free);


/**
 * Checks if all addresses of allocation log where dealocatted
 * @param allocation_log  Address to a allocation log
 * @return A bool representing if allocation log is valid (no memory leaks)
 */ 
Bool valid_allocation_log(MAP2CHECK_CONTAINER* allocation_log);


#endif