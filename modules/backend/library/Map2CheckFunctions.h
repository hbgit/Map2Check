/**********************************************************************
 * This file describes all methods that are instrumented on C programs *
 ***********************************************************************/
#ifndef Map2CheckFunctions_H
#define Map2CheckFunctions_H

#include "AllocationLog.h"
#include "Container.h"
#include "KleeLog.h"
#include "ListLog.h"
#include "HeapLog.h"
#include "Map2CheckGlobals.h"
#include "Map2CheckTypes.h"
#include "PropertyGenerator.h"

/** Initializes variables used in map2check */
void map2check_init(int isSvComp);

/** Finalizes and exit program with error */
void map2check_error();


/**
 * @brief Checks if current state of program is a deref error
 * @param line           Line where deref error occurred
 * @param function_name  Name of the function where operation occurred
 */
void map2check_check_deref(int line, const char* function_name);

void map2check_klee_int(unsigned line, unsigned scope, int value, const char* function_name);
/**
 * Track pointer pointer for LIST_LOG (this function is to be used for instrumentation)
 * @param x     Address of current pointer
 * @param scope Number of the scope
 * @param name  Name of the pointer
 * @param line  Line where pointer was declared
 */
void map2check_pointer(void* x, unsigned scope, const char* name, int line);

/**
 * Track pointer store operations (this function is to be used for instrumentation)
 * @param var   Address of current pointer
 * @param value New address to where pointer points to
 * @param scope Number of the scope
 * @param name  Name of the pointer
 * @param line  Line where store operation was called
 * @param function_name  Name of the function where the operation occured.
 */
void map2check_add_store_pointer(void* var, void* value, unsigned scope, const char* name, int line, const char* function_name);

/**
 * Generates a non det integer by using an undirect KLEE call (this function is to be used for instrumentation)
 * @return An integer representing a non deterministic value
 */
int map2check_non_det_int();

/**
 * Tracks execution of a specific function (this function is to be used for instrumentation)
 * @param func_name   Name of the function where the operation occured.
 * @param scope       Number of the scope
 * @param line        Line where store operation was called
 */
void map2check_target_function(const char* func_name, int scope, int line);

/**
 * Tracks address that are resolved during free (this function is to be used for instrumentation)
 * @param ptr         Address to be released
 * @param line        Line where store operation was called
 * @param func_name   Name of the function where the operation occured.
 * @param isNullValid If value is not 0, ignores if ptr points to NULL
 */
void map2check_free_resolved_address(void* ptr, unsigned line, const char* function_name, short int isNullValid);

/**
 * Tracks address that are resolved during free (this function is to be used for instrumentation)
 * @param ptr         Address allocated
 * @param size        Size of the allocated addres
 */
void map2check_malloc(void* ptr, int size);

/**
 * Tracks address that are resolved during free (this function is to be used for instrumentation)
 * @param ptr         Address allocated
 * @param size        Size of the allocated addres
 */
void map2check_posix(void* ptr, int size);


/**
 * Tracks address that are resolved during free (this function is to be used for instrumentation)
 * @param ptr         Address allocated
 * @param size        Size of the allocated addres
 */
void map2check_calloc(void* ptr, int quantity, int size);

/**
 * Tracks address that are freeed (this function is to be used for instrumentation)
 * @param name        Name of the variable
 * @param ptr         Address to be released
 * @param scope       Number of the scope
 * @param size        Size of the allocated addres
 * @param line        Line where store operation was called
 * @param func_name   Name of the function where the operation occured.
 */
void map2check_free(const char* name, void* ptr, unsigned scope, unsigned line,  const char* function_name);


/**
 * @brief Checks if address where the var will be stored is valid
 * @param ptr               Pointer to where value will be stored
 * @param size_of_destiny   Size of the value to be stored
 */
void map2check_store(void* ptr, unsigned size_of_destiny);

/**
 * @brief Checks if address to be loaded is valid
 * @param ptr    Pointer to where value will be loaded
 * @param size   Size of the type to be loaded
 */
void map2check_load(void* ptr, int size);

/**
 * @brief Tracks addresses from memory heap
 * @param name               Name of the variable allocated
 * @param ptr                Memory address allocated
 * @param size               Size of the allocated address
 * @param size_of_primitive  Size of the primitive on allocated address
 */
void map2check_alloca(const char* name, void* ptr, int size, int size_of_primitive, int line, int scope);
void map2check_non_static_alloca(const char* name, void* ptr, int size, int size_of_primitive, int line, int scope);

char map2check_non_det_char();
void map2check_klee_char(unsigned line, unsigned scope, int value, const char* function_name);

void* map2check_non_det_pointer();
long map2check_non_det_long();
unsigned short map2check_non_det_ushort();
void map2check_klee_pointer(unsigned line, unsigned scope, int value, const char* function_name);
void map2check_assume(int expr);
void __VERIFIER_error();
/**
 * @brief Tracks addresses from memory heap
 * @param name Name of the variable allocated
 * @param ptr  Memory address allocated
 */
void map2check_function(const char* name, void* ptr);

/**
 *
 * Generates TRUE result
 */
void map2check_success();

/**
 * Releases allocated resources and generates property violated 
 */
void map2check_exit();


/**
 * Updates all variables that points to the address with new status 
 * @param address  Address to be updates
 * @param status  Current memory status
 * @param line  Line where operation happened
 */ 
void update_reference_list_log(long address, enum MemoryAddressStatus status, unsigned line);

#endif
