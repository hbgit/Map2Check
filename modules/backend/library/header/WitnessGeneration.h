#ifndef WitnessGeneration_H
#define WitnessGeneration_H

#include "Map2CheckTypes.h"
/**
 * Generate files containing info to generate witness on frontend
 * @param non_det_log   Container with non deterministic log
 * @param trackbb_log Container with trackbb_log
 */
void generate_aux_files(MAP2CHECK_CONTAINER *trackbb_log);

#endif
