/**********************************************************************
 * This file describes all methods that are instrumented on C programs *
 ***********************************************************************/
#ifndef Map2CheckFunctions_H
#define Map2CheckFunctions_H

#include "Map2CheckTypes.h"

/** Initializes variables used in map2check */
void map2check_init(int isSvComp);

/** Finalizes and exit program with error */
void map2check_error();

void map2check_track_bb(unsigned line, const char *function_name);

/**
 * Track pointer pointer for LIST_LOG (this function is to be used for
 * instrumentation)
 * @param x     Address of current pointer
 * @param scope Number of the scope
 * @param name  Name of the pointer
 * @param line  Line where pointer was declared
 */
void map2check_pointer(void *x, unsigned scope, const char *name, int line);

/**
 * Tracks execution of a specific function (this function is to be used for
 * instrumentation)
 * @param func_name   Name of the function where the operation occurred.
 * @param scope       Number of the scope
 * @param line        Line where store operation was called
 */
void map2check_target_function(const char *func_name, int scope, int line);

/**
 * @brief Checks if address where the var will be stored is valid
 * @param ptr               Pointer to where value will be stored
 * @param size_of_destiny   Size of the value to be stored
 */
void map2check_store(void *ptr, unsigned size_of_destiny);

void map2check_assume(int expr);
void map2check_assume_loop(char expr);
void __VERIFIER_error();

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
 * Releases allocated resources
 */
void map2check_destroy();

/**
 * Returns current step for map2check (used to generate wintess/example)
 */
unsigned map2check_get_current_step();
/**
 * Increments current step
 */
void map2check_next_current_step();
#endif
