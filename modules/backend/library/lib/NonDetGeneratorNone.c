/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include "../header/NonDetGenerator.h"

/* This mode returns consts */

#include <stdint.h>
extern int __map2check_main__();

int main() {
  __map2check_main__();
  return 0;
}

int map2check_non_det_int() { return 0; }

unsigned int map2check_non_det_uint() { return 0; }

char map2check_non_det_char() { return 0; }

unsigned short map2check_non_det_ushort() { return 0; }

short map2check_non_det_short() { return 0; }

double map2check_non_det_double() { return 0; }

void *map2check_non_det_pointer() {
  // Ignore warnings from this, it is desired to be undefined behaviour
  return 0;
}

long map2check_non_det_long() { return 0; }
void nondet_assume(int expr) {}
void nondet_init() {}
void nondet_destroy() {}
void nondet_generate_aux_witness_files() {}
