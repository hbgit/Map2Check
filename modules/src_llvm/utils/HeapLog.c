#include "HeapLog.h"

Bool mark_heap_log(MAP2CHECK_CONTAINER* heap_log, MEMORY_HEAP_ROW row) {
    if(heap_log->type != HEAP_LOG_CONTAINER) {
        return FALSE;
    }

    return append_element(heap_log, &row);
}

MEMORY_HEAP_ROW new_memory_row(int line, int scope, void* address, int size, int size_of_primitive, const char* function_name) {
    MEMORY_HEAP_ROW row;
    row.function_name = function_name;
    row.line = line;
    row.scope = scope;
    row.value = address;
    row.size = size;
    row.size_of_primitive  = size_of_primitive;
    return row;
}

/* Same ideia of is_valid_allocation_address from AllocationLog.c,
 * but the main difference is: there is no need to check if address is free
 */
Bool is_valid_heap_address(MAP2CHECK_CONTAINER* heap_log, void* address, long* last_address) {
    printf("Checking for Heap Deref\n");
    int i = heap_log->size - 1;
    long addressToCheck = (long) address;
    for(; i >= 0; i--) {
        MEMORY_HEAP_ROW* iRow = (MEMORY_HEAP_ROW*) get_element_at(i, *heap_log);
        long addressBottom = (long) iRow->value;
        long addressTop = addressBottom + iRow->size;
        if(address <= addressToCheck < addressTop) {
            *last_address = addressTop;
            return TRUE;
        }
    }
    *last_address = 0;
    return FALSE;
}
