/**********************************************************************
 * This file contains all methods that are used for the ListLog which  *
 * is a structure that contains all pointer operations
 ***********************************************************************/

#ifndef ListLog_H
#define ListLog_H


#include "Map2CheckTypes.h"
#include "Container.h"
#include "Map2CheckGlobals.h"



long get_old_reference(long var_address, MAP2CHECK_CONTAINER* log);
Bool is_deref_error(long address, MAP2CHECK_CONTAINER* log);

Bool is_invalid_free(long address, MAP2CHECK_CONTAINER* log);


/**
 * This method initializes a LIST_LOG_ROW with the defined parameters
 * @param  memory_address           Memory Address to the pointer var
 * @param  memory_address_points_to Memory Addres to where the pointer points to
 * @param  scope                    Scope of the pointer
 * @param  is_dynamic               Is the pointer DYNAMIC? (received address from malloc, realloc, calloc)
 * @param  is_free                  Is the pointer freed?
 * @param  line_number              Line number where the operation occured (0 means that the information is lost)
 * @param  name                     Name for the pointer var
 * @param  function_name            Name of the current function
 * @return                          Initialized LIST_LOG_ROW
 */
LIST_LOG_ROW new_list_row  (long memory_address, long memory_address_points_to,
			    unsigned scope, Bool is_dynamic, Bool is_free, unsigned line_number,
			    const char* name, const char* function_name, unsigned step);

/**
 * Write elements from LIST_LOG to a FILE
 * @param row Pointer to the LIST_LOG to be writed
 */
void list_log_to_file(MAP2CHECK_CONTAINER* list);


/**
 * Prints elements from a LIST_LOG
 * @param list Pointer to the LIST_LOG to be printed
 */
void print_list_log(MAP2CHECK_CONTAINER* list);

enum MemoryAddressStatus get_type_from_list_log_row(LIST_LOG_ROW* row);


#endif
