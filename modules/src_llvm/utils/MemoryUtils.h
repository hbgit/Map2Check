#ifndef MEMORY_ROW_H
#define MEMORY_ROW_H

#define bool char
#define true 1
#define false 0

#define MEMORY char
#define STATIC 0
#define FREE 1
#define DYNAMIC 2

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
  long addr;
  bool is_free;
} MEMORY_ALLOCATIONS_ROW;

typedef struct obj3 {
  MEMORY_ALLOCATIONS_ROW* values;
  unsigned size;
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
bool mark_allocation_log(MEMORY_ALLOCATIONS_LOG* allocation_log, long address);

/**
 * Mark element as free to a MEMORY_ALLOCATIONS_LOG
 * @param  allocation_log Pointer to the MEMORY_ALLOCATIONS_LOG
 * @param  address        Memory Address to be freed
 * @return                Returns success of operation
 */
bool mark_deallocation_log(MEMORY_ALLOCATIONS_LOG* allocation_log, long address);

/**
 * Checks if an address exists in a MEMORY_ALLOCATIONS_LOG, it checks from the last added to the first
 * @param  allocation_log Pointer to the MEMORY_ALLOCATIONS_LOG
 * @param  address        Memory Address to be checked
 * @return                Returns the most recent status of the address
 */
MEMORY check_address_allocation_log(MEMORY_ALLOCATIONS_LOG* allocation_log, long address);

/**
 * Free allocated values from MEMORY_ALLOCATIONS_LOG
 * @param  allocation_log Pointer to the MEMORY_ALLOCATIONS_LOG
 * @return                Returns the success of operation
 */
bool free_memory_allocation_log(MEMORY_ALLOCATIONS_LOG* allocation_log);

void print_allocation_log(MEMORY_ALLOCATIONS_LOG* allocation_log);
/**
 * This method initializes a MEMORY_ALLOCATIONS_ROW with the defined parameters
 * @param  address Memory Address to be logged
 * @param  is_free Is the *address* released?
 * @return         Returns the initialized MEMORY_ALLOCATIONS_ROW
 */
MEMORY_ALLOCATIONS_ROW new_memory_row(long address, bool is_free);


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
 * Write elements from LIST_LOG to a FILE
 * @param row Pointer to the LIST_LOG to be writed
 */
void list_log_to_file(LIST_LOG* list);

/**
 * Prints elements from a LIST_LOG
 * @param list Pointer to the LIST_LOG to be printed
 */
void print_list_log(LIST_LOG* list);

void map2check_pointer(void* x, unsigned scope, const char* name, int line);
void map2check_add_store_pointer(void* var, void* value, unsigned scope, const char* name, int line);

#endif
