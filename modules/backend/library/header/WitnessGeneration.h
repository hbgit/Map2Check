/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#ifndef WitnessGeneration_H
#define WitnessGeneration_H

#include "Map2CheckTypes.h"
/**
 * Generate files containing info to generate witness on frontend
 * @param trackbb_log Container with trackbb_log
 * @param violation   TRUE when this execution violated the property.
 *
 * The nondet log is only flushed for a violating execution. Under KLEE every
 * forked state runs this on exit and they all write the same file, so the last
 * one to finish wins -- and it is usually not the violating one, which aborts
 * early. Guarding the flush is what keeps the violating execution's input
 * vector from being overwritten by a state that took another branch.
 */
void generate_aux_files(MAP2CHECK_CONTAINER *trackbb_log, Bool violation);

/**
 * Generate a error file to confirm that map2check really crashed with input
 */
void generate_confirmation_file();
#endif
