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
  const char* scope;
  bool is_dynamic;
  bool is_free;
  unsigned line_number;
} LIST_LOG_ROW;

typedef struct obj1 {
  LIST_LOG_ROW* values;
  int size;
} LIST_LOG;


// Returns LIST_LOG_ROW
LIST_LOG_ROW new_list_row  (long memory_address, long memory_address_points_to,
  const char* scope, bool is_dynamic, bool is_free, unsigned line_number, const char* name);


// Returns allocated LIST_LOG
LIST_LOG new_list_log();

// Adds new item to list and return if operation succeded
bool mark_map_log(LIST_LOG* list, LIST_LOG_ROW* row);

bool free_list_log(LIST_LOG* list);

void debug_list_log(LIST_LOG* list);

void map2check_pointer(void* x, const char* name);

void list_log_to_file(LIST_LOG_ROW* row);
#endif
