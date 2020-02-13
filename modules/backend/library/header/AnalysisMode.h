/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#ifndef AnalysisMode_H
#define AnalysisMode_H

#include "Map2CheckTypes.h"
/** Initializes variables used during analysis */
void analysis_init();

/** Frees all variables used during analysis */
void analysis_destroy();

/** Checks if program is ok. This is the last analysis before current test case
 * terminates */
Bool analysis_is_program_correct();

/** Creates files to be used on a witness generator*/
void analysis_generate_aux_witness_files();

#endif
