#ifndef WitnessGeneration_H
#define WitnessGeneration_H

#include "Map2CheckTypes.h"
/**
 * Generate files containing info to generate witness on frontend
 * @param non_det_log   Container with non deterministic log
 * @param list_log  Container with list log
 * @param heap_log  Container with heap log
 * @param trackbb_log Container with trackbb_log
 * @param allocation_log Container with allocation log
 */
void generate_aux_files(MAP2CHECK_CONTAINER* non_det_log,
    MAP2CHECK_CONTAINER* list_log,
    MAP2CHECK_CONTAINER* heap_log,
    MAP2CHECK_CONTAINER* trackbb_log,
    MAP2CHECK_CONTAINER* allocation_log);

#endif
