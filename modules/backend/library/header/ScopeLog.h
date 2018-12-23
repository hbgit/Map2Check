/**********************************************************************
 * This file contains all methods that are used for the ScopeLog      *
 * which is a structure that contains all scope tracking       *
 ***********************************************************************/
#include "Container.h"
#include "Map2CheckTypes.h"

#ifndef SCOPELOG_H
#define SCOPELOG_H

/**
 * Mark element as scope to a MAP2CHECK_CONTAINER if it exists, it will be
 * updated
 * @param  container      Pointer to the MAP2CHECK_CONTAINER
 * @param  row            Row of the SCOPE_ROW to be added on the
 * container
 * @return                Returns success of operation
 */
Bool mark_scope_log(MAP2CHECK_CONTAINER* container, SCOPE_ROW row);

/**
 * Checks if address is valid in current scope
 * updated
 * @param  container      Pointer to the MAP2CHECK_CONTAINER
 * @param  actual         SCOPE_ROW with current ID
 * @param  toCheck        SCOPE_ROW with ID to check
 * @return                Returns success of operation
 */
Bool is_scope_valid(MAP2CHECK_CONTAINER* container, SCOPE_ROW row,
                    SCOPE_ROW toCheck);

/**
 * This method initializes a SCOPE_ROW with the defined parameters
 * @param  id                 ID of current scope
 * @param  parent             Parent of current scope
 * @param  function_name        Name of the function where operation took place
 * @return                      Returns the initialized SCOPE_ROW
 */
SCOPE_ROW new_heap_row(unsigned id, unsigned parent, const char* function_name);

#endif