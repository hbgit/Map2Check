/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

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
                            unsigned scope, void *value,
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

void map2check_nondet_double(unsigned line, unsigned scope, double value,
                           const char *function_name);

void map2check_nondet_float(unsigned line, unsigned scope, double value,
                            const char *function_name);

void map2check_nondet_longlong(unsigned line, unsigned scope, long long value,
                               const char *function_name);

void map2check_nondet_ulonglong(unsigned line, unsigned scope,
                                unsigned long long value,
                                const char *function_name);

void map2check_nondet_u8(unsigned line, unsigned scope, int value,
                         const char *function_name);

void map2check_nondet_u16(unsigned line, unsigned scope, int value,
                          const char *function_name);

void map2check_nondet_u32(unsigned line, unsigned scope, int value,
                          const char *function_name);

/** Logs one byte of a __VERIFIER_nondet_memory call. Called by the runtime's
 * map2check_non_det_memory, not by NonDetPass: the bytes are read inside it. */
void map2check_nondet_memory_byte(unsigned line, unsigned scope, int value,
                                  const char *function_name);


#endif
