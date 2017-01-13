#ifndef MEMORY_ROW_H
#define MEMORY_ROW_H

#define bool char
#define true 1
#define false 0

typedef struct obj {
  unsigned id;
  const char* var_name;
  long memory_address;
  long memory_address_points_to;
  unsigned scope;
  bool is_dynamic;
  bool is_free;
  unsigned line_number;
} LIST_LOG_ROW;

typedef struct obj1 {
  LIST_LOG_ROW* values;
  int size;
} LIST_LOG;

typedef struct obj2 {
  struct obj2* next;
  // struct obj2* prev;
  long addr;
  bool is_free;
} MEMORY_ALLOCATIONS_LOG;

/**
 * Creates a new MEMORY_ALLOCATIONS_LOG
 * @return A new MEMORY_ALLOCATIONS_LOG pre-initialized
 */
MEMORY_ALLOCATIONS_LOG new_memory_allocation();

/**
 * Mark element as not free to a MEMORY_ALLOCATIONS_LOG
 * @param  allocation_log Pointer to the MEMORY_ALLOCATIONS_LOG
 * @param  address        Memory Address to be added
 * @return                Returns success of operation
 */
bool mark_allocation_log(MEMORY_ALLOCATIONS_LOG* allocation_log, long address, bool is_free);

/**
 * Mark element as free to a MEMORY_ALLOCATIONS_LOG
 * @param  allocation_log Pointer to the MEMORY_ALLOCATIONS_LOG
 * @param  address        Memory Address to be freed
 * @return                Returns success of operation
 */
bool mark_deallocation_log(MEMORY_ALLOCATIONS_LOG* allocation_log, long address);

/**
 * Checks if an address exists in a MEMORY_ALLOCATIONS_LOG
 * @param  allocation_log Pointer to the MEMORY_ALLOCATIONS_LOG
 * @param  address        Memory Address to be checked
 * @return                Returns if the Memory Address exists in the MEMORY_ALLOCATIONS_LOG
 */
bool check_address_allocation_log(MEMORY_ALLOCATIONS_LOG* allocation_log, long address);


/**
 * This method initializes a LIST_LOG_ROW with the defined parameters
 * @param  memory_address           Memory Address to the pointer var
 * @param  memory_address_points_to Memory Addres to where the pointer points to
 * @param  scope                    Scope of the pointer
 * @param  is_dynamic               Is the pointer DYNAMIC? (received address from malloc, realloc, calloc)
 * @param  is_free                  Is the pointer freed?
 * @param  line_number              Line number where the operation occured (0 means that the information is lost)
 * @param  name                     Name for the pointer var
 * @return                          Initialized LIST_LOG_ROW
 */
LIST_LOG_ROW new_list_row  (long memory_address, long memory_address_points_to,
  unsigned scope, bool is_dynamic, bool is_free, unsigned line_number, const char* name);

/**
 * Initializes a LIST_LOG
 * @return A new LIST_LOG
 */
LIST_LOG new_list_log();

/**
 * Mark a new element for a LIST_LOG
 * @param  list Pointer to the LIST_LOG that will receive a new LIST_LOG_ROW
 * @param  row  Pointer to the LIST_LOG_ROW that will be added
 * @return      Returns the success of the operation
 */
bool mark_map_log(LIST_LOG* list, LIST_LOG_ROW* row);

/**
 * Free used memory from LIST_LOG
 * @param  list Pointer to the LIST_LOG to be freed
 * @return      Returns the success of the operation
 */
bool free_list_log(LIST_LOG* list);

/**
 * Append elements from LIST_LOG_ROW to a FILE
 * @param row Pointer to the LIST_LOG_ROW to be appended
 */
void list_log_to_file(LIST_LOG_ROW* row);

/**
 * Prints elements from a LIST_LOG
 * @param list Pointer to the LIST_LOG to be printed
 */
void print_list_log(LIST_LOG* list);

void map2check_pointer(void* x, unsigned scope, const char* name, int line);
void map2check_add_store_pointer(void* var, void* value, unsigned scope, const char* name, int line);

#endif
