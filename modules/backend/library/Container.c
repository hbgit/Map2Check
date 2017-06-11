#include "Container.h"

#include <stdio.h>
#include <stdlib.h>

MAP2CHECK_CONTAINER new_container(enum Container_Type type) {
    MAP2CHECK_CONTAINER container;
    container.size = 0;
    container.values = NULL;
    container.type = type;
    return container;
}

Bool free_container(MAP2CHECK_CONTAINER* container) {
    switch (container->type) {
        case LIST_LOG_CONTAINER:
            free((LIST_LOG_ROW*)container->values);
            break;
        case ALLOCATION_LOG_CONTAINER:
            free((MEMORY_ALLOCATIONS_ROW*)container->values);
            break;
        case KLEE_LOG_CONTAINER:
            free((KLEE_CALL*)container->values);
            break;
        case HEAP_LOG_CONTAINER:
            free((MEMORY_HEAP_ROW*)container->values);
            break;
        }

    return TRUE;
}

Bool append_element(MAP2CHECK_CONTAINER* container, void* row) {
    container->size += 1;
    
    int new_allocation_size; 

    switch(container->type) {
        case LIST_LOG_CONTAINER:
            new_allocation_size = container->size * sizeof(LIST_LOG_ROW);
            break;
        case ALLOCATION_LOG_CONTAINER:
            new_allocation_size = container->size * sizeof(MEMORY_ALLOCATIONS_ROW);
            break;   
        case KLEE_LOG_CONTAINER:
            new_allocation_size = container->size * sizeof(KLEE_CALL);
            break;        
        case HEAP_LOG_CONTAINER:
            new_allocation_size = container->size * sizeof(MEMORY_HEAP_ROW);
            break;
    }
    // printf("new allocation size: %d\n", new_allocation_size);
    void* temp_list = realloc(container->values, new_allocation_size);
    
    LIST_LOG_ROW* list;
    MEMORY_ALLOCATIONS_ROW* allocationLog;
    MEMORY_HEAP_ROW* heapLog;
    KLEE_CALL* kleeLog;
    switch(container->type) {
        case LIST_LOG_CONTAINER:
            list = (LIST_LOG_ROW*) temp_list;
            list[container->size - 1] = *((LIST_LOG_ROW*) row);                        
            break;
        case ALLOCATION_LOG_CONTAINER:
            allocationLog = (MEMORY_ALLOCATIONS_ROW*) temp_list;            
            allocationLog[container->size - 1] = *((MEMORY_ALLOCATIONS_ROW*) row);            
            break; 
        case KLEE_LOG_CONTAINER:
            kleeLog = (KLEE_CALL*) temp_list;
            kleeLog[container->size - 1] = *((KLEE_CALL*) row);
            break;
        case HEAP_LOG_CONTAINER:
            heapLog = (MEMORY_HEAP_ROW*) temp_list;
            heapLog[container->size - 1] = *((MEMORY_HEAP_ROW*) row);
            break;

    }

    container->values = temp_list;
    return TRUE;
}

void* get_element_at(unsigned index, MAP2CHECK_CONTAINER container) {    
    if(index >= container.size ) {    
        return NULL;
    }
    
    LIST_LOG_ROW* listLogRows; 
    MEMORY_ALLOCATIONS_ROW* allocationLog;
    KLEE_CALL* kleeLog;
    MEMORY_HEAP_ROW* heapLog;
    switch(container.type) {
        case LIST_LOG_CONTAINER:            
            listLogRows = (LIST_LOG_ROW*) container.values;
            return (&listLogRows[index]);
        case ALLOCATION_LOG_CONTAINER:            
            allocationLog = (MEMORY_ALLOCATIONS_ROW*) container.values;
            return (&allocationLog[index]);    
        case KLEE_LOG_CONTAINER:            
            kleeLog = (KLEE_CALL*) container.values;
            return (&kleeLog[index]);        
        case HEAP_LOG_CONTAINER:
            heapLog = (MEMORY_HEAP_ROW*) container.values;
            return (&heapLog[index]);
    }
    return NULL;
}
