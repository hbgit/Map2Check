/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#ifndef NonDetGenerator_H
#define NonDetGenerator_H

void nondet_init();
void nondet_destroy();
void nondet_cancel();
void nondet_generate_aux_witness_files();
void nondet_assume(int expr);

// TODO: create macro to generate functions with docs
/**
 * @brief Generates non deterministic integer
 * @return Non deterministic integer
 */
int map2check_non_det_int();

/**
 * @brief Generates non deterministic uint
 * @return Non deterministic uint
 */
unsigned int map2check_non_det_uint();

/**
 * @brief Generates non deterministic char
 * @return Non deterministic char
 */
char map2check_non_det_char();

/**
 * @brief Generates non deterministic ushort
 * @return Non deterministic ushort
 */
typedef unsigned short ushort;
ushort map2check_non_det_ushort();

/**
 * @brief Generates non deterministic short
 * @return Non deterministic short
 */
short map2check_non_det_short();

/**
 * @brief Generates non deterministic ptr
 * @return Non deterministic ptr
 */
typedef void *pointer;
pointer map2check_non_det_pointer();

/**
 * @brief Generates non deterministic long
 * @return Non deterministic long
 */
long map2check_non_det_long();

/**
 * @brief Generates non deterministic unsigned long
 * @return Non deterministic ulong
 */
typedef unsigned long ulong;
unsigned long map2check_non_det_ulong();

/**
 * @brief Generates non deterministic boolean
 * @return Non deterministic bool
 */
typedef int bool;
bool map2check_non_det_bool();

/**
 * @brief Generates non deterministic unsigned
 * @return Non deterministic unsigned
 */
unsigned map2check_non_det_unsigned();

/**
 * @brief Generates non deterministic unsigned char
 * @return Non deterministic unsigned char
 */
typedef unsigned char uchar;
unsigned char map2check_non_det_uchar();

/**
 * @brief Generates non deterministic pchar (terminates with \0)
 * @return Non deterministic char array
 */
char *map2check_non_det_pchar();

#include <stdlib.h>
/**
 * @brief Generates non deterministic size_t
 * @return Non deterministic unsigned size_t
 */
size_t map2check_non_det_size_t();

/**
 * @brief Generates non deterministic loff_t
 * @return Non deterministic unsigned loff_t
 */
// NOTE: intellisense can't work with loff
#ifndef __INTELLISENSE__
loff_t map2check_non_det_loff_t();
#endif

#define sector_t unsigned long
/**
 * @brief Generates non deterministic sector_t
 * @return Non deterministic unsigned sector_t
 */
sector_t map2check_non_det_sector_t();

/**
 * @brief Generates non deterministic unsigned
 * @return Non deterministic unsigned
 */
typedef unsigned uint;
uint map2check_non_det_uint();

/**
 * @brief Generates non deterministic double
 * @return Non deterministic double
 */
double map2check_non_det_double();

#endif
