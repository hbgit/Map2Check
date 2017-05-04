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
    int i;
    // for(i = 0; i < container->size; i++) {
    //     free(container->values[i]);
    // }
    free(container->values);
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
    }
    // printf("new allocation size: %d\n", new_allocation_size);
    void* temp_list = realloc(container->values, new_allocation_size);
    
    LIST_LOG_ROW* list;
    MEMORY_ALLOCATIONS_ROW* allocationLog;
    switch(container->type) {
        case LIST_LOG_CONTAINER:
            list = (LIST_LOG_ROW*) temp_list;
            list[container->size - 1] = *((LIST_LOG_ROW*) row);            
            // printf("address: %p\n", (void*) list[container->size - 1].memory_address_points_to);
            break;
        case ALLOCATION_LOG_CONTAINER:
            allocationLog = (MEMORY_ALLOCATIONS_ROW*) temp_list;
            // printf("address: %p\n", (void*) allocationLog[container->size - 1].addr);
            allocationLog[container->size - 1] = *((MEMORY_ALLOCATIONS_ROW*) row);            
            break;    
    }

    container->values = temp_list;
    return TRUE;
}

void* get_element_at(unsigned index, MAP2CHECK_CONTAINER container) {
    printf("getting element at: %d\n", index)    ;
    if(index >= container.size ) {
        printf("invalid index\n", index)    ;
        return NULL;
    }
    
    LIST_LOG_ROW* listLogRows; 
    MEMORY_ALLOCATIONS_ROW* allocationLog;
    
    switch(container.type) {
        case LIST_LOG_CONTAINER:
            // printf("LIST LOG\n", index)    ;
            listLogRows = (LIST_LOG_ROW*) container.values;
            return (&listLogRows[index]);
        case ALLOCATION_LOG_CONTAINER:
            // printf("ALLOCATION LOG\n", index)    ;
            allocationLog = (MEMORY_ALLOCATIONS_ROW*) container.values;
            return (&allocationLog[index]);    
    }
    printf("error\n", index)    ;
    return NULL;
}