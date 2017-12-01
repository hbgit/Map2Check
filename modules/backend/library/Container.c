#include "Container.h"
#include "BTree.h"
#include <stdio.h>
#include <stdlib.h>

MAP2CHECK_CONTAINER new_container(enum Container_Type type) {
  MAP2CHECK_CONTAINER container;
  container.size = 0;
  B_TREE* btree = malloc(sizeof(B_TREE));
  *btree = B_TREE_CREATE(type);
  container.values = btree;
  container.type = type;
  return container;
}

Bool free_container(MAP2CHECK_CONTAINER* container) {
 
  B_TREE_FREE((B_TREE*) container->values);
  free(container->values);
  return TRUE;
}

Bool append_element(MAP2CHECK_CONTAINER* container, void* row) {
  B_TREE_ROW btrow;
  btrow.index = container->size;
  container->size += 1;    

  switch(container->type) {
  case LIST_LOG_CONTAINER:
    btrow.value.listLog = *((LIST_LOG_ROW*) row);
    break;
  case ALLOCATION_LOG_CONTAINER:
    btrow.value.allocationLog = *((MEMORY_ALLOCATIONS_ROW*) row);
    break; 
  case KLEE_LOG_CONTAINER:
    btrow.value.kleeLog = *((KLEE_CALL*) row);
    break;
  case HEAP_LOG_CONTAINER:
    btrow.value.heapLog = *((MEMORY_HEAP_ROW*) row);
    break;
  case TRACKBB_LOG_CONTAINER:
    btrow.value.trackBBLog = *((TRACK_BB_ROW*) row);
    break;
  }
  B_TREE_INSERT(container->values, &btrow);
  return TRUE;
}

void* get_element_at(unsigned index, MAP2CHECK_CONTAINER container) {    
  if(index >= container.size ) {    
    return NULL;
  } 

  B_TREE_ROW* row = B_TREE_SEARCH(container.values, index);  
  return &row->value;
}
