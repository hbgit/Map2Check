/**********************************************************************
 * This file contains all methods that are used for the KleeLog which  *
 * is a structure that contains all klee operations
 ***********************************************************************/

#include "Container.h"
#include "Map2CheckGlobals.h"
#include "Map2CheckTypes.h"

#ifndef NonDetLog_H
#define NonDetLog_H

/**
 * Generates file containing Klee Log
 * @param klee_container  Pointer to Container
 * @return                A bool representing success of operation
 */
Bool nondet_log_to_file(MAP2CHECK_CONTAINER klee_container);

/**
 * Initializes a klee call
 * @param type
 * @param line
 * @param scope
 * @param value
 * @param function_name
 * @param step
 */
NONDET_CALL new_nondet_call(enum NONDET_TYPE type, unsigned line,
                            unsigned scope, long value,
                            const char *function_name, unsigned step);

void nondet_log_init();
void nondet_log_destroy();
MAP2CHECK_CONTAINER map2check_nondet_get_log();
void map2check_nondet_int(unsigned line, unsigned scope, int value,
                          const char *function_name);
void map2check_nondet_unsigned(unsigned line, unsigned scope, unsigned value,
                               const char *function_name);
void map2check_nondet_char(unsigned line, unsigned scope, int value,
                           const char *function_name);

void map2check_nondet_ulong(unsigned line, unsigned scope, int value,
                            const char *function_name);

void map2check_nondet_pointer(unsigned line, unsigned scope, int value,
                              const char *function_name);

void map2check_nondet_bool(unsigned line, unsigned scope, int value,
                           const char *function_name);

void map2check_nondet_uchar(unsigned line, unsigned scope, int value,
                            const char *function_name);

void map2check_nondet_pchar(unsigned line, unsigned scope, int value,
                            const char *function_name);

void map2check_nondet_size_t(unsigned line, unsigned scope, int value,
                             const char *function_name);

void map2check_nondet_loff_t(unsigned line, unsigned scope, int value,
                             const char *function_name);

void map2check_nondet_sector_t(unsigned line, unsigned scope, int value,
                               const char *function_name);

void map2check_nondet_uint(unsigned line, unsigned scope, int value,
                           const char *function_name);

void map2check_nondet_short(unsigned line, unsigned scope, int value,
                           const char *function_name);


#endif
