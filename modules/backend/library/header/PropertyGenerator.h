/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

/**********************************************************************
 * This file describes all methods that are used for generation of the *
 * file containing the property that was violated (or none)
 ***********************************************************************/

#ifndef PropertyGenerator_H
#define PropertyGenerator_H

#include "Map2CheckTypes.h"
/**
 * Generate a file containing the violated property
 * @param violated   Violated Property of program
 * @param line  Line where store operation was called
 * @param function_name  Name of the function where the operation occurred.
 */
void write_property(enum ViolatedProperty violated, int line,
                    const char *function_name);
void write_property_unknown();
void write_property_deref(int line, const char *function_name);
void write_property_memtrack();
void write_property_memcleanup();
void write_property_free(int line, const char *function_name);
void write_property_overflow(int line, const char *function_name);
void write_property_assert(int line, const char *function_name);

#endif
