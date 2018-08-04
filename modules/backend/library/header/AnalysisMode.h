#ifndef AnalysisMode_H
#define AnalysisMode_H

#include "Map2CheckTypes.h"
/** Initializes variables used during analysis */
void analysis_init();

/** Frees all variables used during analysis */
void analysis_destroy();

/** Checks if program is ok */
Bool analysis_is_program_correct();

/** Creates files to be used on a witness generator*/
void analysis_generate_aux_witness_files();

#endif
