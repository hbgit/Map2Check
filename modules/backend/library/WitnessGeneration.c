#include "WitnessGeneration.h"
#include "KleeLog.h"
#include "ListLog.h"
#include "HeapLog.h"
#include "TrackBBLog.h"
#include "AllocationLog.h"

void generate_aux_files(MAP2CHECK_CONTAINER* non_det_log,
    MAP2CHECK_CONTAINER* list_log,
    MAP2CHECK_CONTAINER* heap_log,
    MAP2CHECK_CONTAINER* trackbb_log,
    MAP2CHECK_CONTAINER* allocation_log) {
    klee_log_to_file(*non_det_log);
    list_log_to_file(list_log);
    heap_log_to_file(heap_log);
    trackbb_log_to_file(trackbb_log);
    allocation_log_to_file(allocation_log);
}
