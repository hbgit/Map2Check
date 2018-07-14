/**********************************************************************
 * This file contains all types and data structures for Map2Check
 ***********************************************************************/

#ifndef Map2CheckTypes_H
#define Map2CheckTypes_H

#define Bool char
#define FALSE 0
#define TRUE 1

#define FUNCTION_MAX_LENGTH_NAME 35

// TODO(rafa.sa.xp@gmail.com) Should use static array length for function names

enum PRIMITIVE_TYPE {
  PRIMITIVE_INTEGER = 0,
  PRIMITIVE_CHAR = 1,
  PRIMITIVE_POINTER = 2,
  PRIMITIVE_USHORT = 3,
  PRIMITIVE_SHORT = 4,
  PRIMITIVE_UNSIGNED = 5
};

enum MemoryAddressStatus {
  STATIC = 0,
  FREE = 1,
  DYNAMIC = 2
  //  INVALID = 3
};

enum NONDET_TYPE {
  INTEGER = 0,
  CHAR = 1,
  POINTER = 2,
  USHORT = 3,
  LONG = 4,
  UNSIGNED = 5
};

enum Container_Type {
  LIST_LOG_CONTAINER,
  HEAP_LOG_CONTAINER,
  ALLOCATION_LOG_CONTAINER,
  KLEE_LOG_CONTAINER,
  TRACKBB_LOG_CONTAINER
};

enum ViolatedProperty {
  NONE = 0,
  UNKNOWN = 1,
  FALSE_FREE = 2,
  FALSE_DEREF = 3,
  FALSE_MEMTRACK = 4,
  TARGET_REACHED = 5,
  OVERFLOW = 6
};

typedef
    /**
     * Struct to save ROWS form LIST_LOG
     */
    struct obj {
  /** Current ID on ListLog */
  unsigned id;
  /** Variable name */
  const char* var_name;
  /** Address of the pointer */
  long memory_address;
  /** Address to where the pointer points to */
  long memory_address_points_to;
  /** Current scope number (llvm ir uses numbered scopes) */
  unsigned scope;
  /** True if address to where the pointer points to is dynamic */
  Bool is_dynamic;
  /** True if address to where the pointer points to is released */
  Bool is_free;
  /** Number where operation took place (on the C source) */
  unsigned line_number;
  /** Name of the function where operation took place */
  char function_name[FUNCTION_MAX_LENGTH_NAME];
  /**Current step on execution (represent order on which operation occurred) */
  unsigned step_on_execution;
} LIST_LOG_ROW;

typedef
    /**
     * Struct to save ROWS from MEMORY_ALLOCATIONS
     */
    struct obj2 {
  /** Current ID on AllocationLog */
  unsigned id;
  /** Address of the memory */
  long addr;
  /** True if address is released */
  Bool is_free;
  /** Size of the allocated address */
  unsigned size;
} MEMORY_ALLOCATIONS_ROW;

typedef
    /**
     * Struct to save ROWS from KLEE_LOG
     */
    struct obj3 {
  /** Current ID on KleeLog */
  unsigned id;
  /** Type of the value generated */
  enum NONDET_TYPE type;
  /** Line where operation occurred */
  unsigned line;
  /**Current step on execution (represent order on which operation occurred) */
  unsigned step_on_execution;
  /** Current scope number (llvm ir uses numbered scopes) */
  unsigned scope;
  // TODO(rafa.sa.xp@gmail.com) Should update to work with a union
  /** Pointer where the generated value is stored */
  long value;
  /** Name of the function where operation took place */
  char function_name[FUNCTION_MAX_LENGTH_NAME];
} KLEE_CALL;

typedef
    /**
     * Struct to represent the container
     */
    struct obj4 {
  /** Type of the container generated */
  enum Container_Type type;
  /** Size of the container (number of rows) */
  unsigned size;
  /** Pointer to structure holding the values */
  void* values;
} MAP2CHECK_CONTAINER;

typedef
    /**
     * Struct to represent the Heap Log
     */
    struct obj5 {
  /** Line where operation occurred */
  unsigned line;
  /** Current scope number (llvm ir uses numbered scopes) */
  unsigned scope;
  /** Pointer where the generated value is stored */
  void* value;
  /** Size of the memory address */
  int size;
  /** Size of primitives */
  int size_of_primitive;
  /** Name of the function where operation took place */
  char function_name[FUNCTION_MAX_LENGTH_NAME];
} MEMORY_HEAP_ROW;

typedef
    /**
     * Struct to represent the Track BB Log
     */
    struct obj6 {
  /** Line where operation occurred */
  unsigned line;
  /** Name of the function where operation took place */
  char function_name[FUNCTION_MAX_LENGTH_NAME];
} TRACK_BB_ROW;

typedef union CONTAINER_ROW {
  LIST_LOG_ROW listLog;
  MEMORY_ALLOCATIONS_ROW allocationLog;
  MEMORY_HEAP_ROW heapLog;
  TRACK_BB_ROW trackBBLog;
  KLEE_CALL kleeLog;
} CONTAINER_ROW;

#endif
