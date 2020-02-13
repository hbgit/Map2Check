/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include "../header/Container.h"

#include <stdio.h>
#include <stdlib.h>

MAP2CHECK_CONTAINER new_container(enum Container_Type type) {
  MAP2CHECK_CONTAINER container;
  container.size = 0;
  container.values = NULL;
  container.type = type;
  return container;
}

Bool free_container(MAP2CHECK_CONTAINER *container) {
  switch (container->type) {
  case LIST_LOG_CONTAINER:
    free((LIST_LOG_ROW *)container->values);
    break;
  case ALLOCATION_LOG_CONTAINER:
    free((MEMORY_ALLOCATIONS_ROW *)container->values);
    break;
  case NONDET_LOG_CONTAINER:
    free((NONDET_CALL *)container->values);
    break;
  case HEAP_LOG_CONTAINER:
    free((MEMORY_HEAP_ROW *)container->values);
    break;
  case TRACKBB_LOG_CONTAINER:
    free((TRACK_BB_ROW *)container->values);
    break;
  }

  return TRUE;
}

Bool append_element(MAP2CHECK_CONTAINER *container, void *row) {
  container->size += 1;

  int new_allocation_size;

  switch (container->type) {
  case LIST_LOG_CONTAINER:
    new_allocation_size = container->size * sizeof(LIST_LOG_ROW);
    break;
  case ALLOCATION_LOG_CONTAINER:
    new_allocation_size = container->size * sizeof(MEMORY_ALLOCATIONS_ROW);
    break;
  case NONDET_LOG_CONTAINER:
    new_allocation_size = container->size * sizeof(NONDET_CALL);
    break;
  case HEAP_LOG_CONTAINER:
    new_allocation_size = container->size * sizeof(MEMORY_HEAP_ROW);
    break;
  case TRACKBB_LOG_CONTAINER:
    new_allocation_size = container->size * sizeof(TRACK_BB_ROW);
    break;
  }
  // printf("new allocation size: %d\n", new_allocation_size);
  void *temp_list = realloc(container->values, new_allocation_size);

  LIST_LOG_ROW *list;
  MEMORY_ALLOCATIONS_ROW *allocationLog;
  MEMORY_HEAP_ROW *heapLog;
  TRACK_BB_ROW *trackBBLog;
  NONDET_CALL *nondetLog;
  switch (container->type) {
  case LIST_LOG_CONTAINER:
    list = (LIST_LOG_ROW *)temp_list;
    list[container->size - 1] = *((LIST_LOG_ROW *)row);
    break;
  case ALLOCATION_LOG_CONTAINER:
    allocationLog = (MEMORY_ALLOCATIONS_ROW *)temp_list;
    allocationLog[container->size - 1] = *((MEMORY_ALLOCATIONS_ROW *)row);
    break;
  case NONDET_LOG_CONTAINER:
    nondetLog = (NONDET_CALL *)temp_list;
    nondetLog[container->size - 1] = *((NONDET_CALL *)row);
    break;
  case HEAP_LOG_CONTAINER:
    heapLog = (MEMORY_HEAP_ROW *)temp_list;
    heapLog[container->size - 1] = *((MEMORY_HEAP_ROW *)row);
    break;
  case TRACKBB_LOG_CONTAINER:
    trackBBLog = (TRACK_BB_ROW *)temp_list;
    trackBBLog[container->size - 1] = *((TRACK_BB_ROW *)row);
    break;
  }

  container->values = temp_list;
  return TRUE;
}

void *get_element_at(unsigned index, MAP2CHECK_CONTAINER container) {
  if (index >= container.size) {
    return NULL;
  }

  LIST_LOG_ROW *listLogRows;
  MEMORY_ALLOCATIONS_ROW *allocationLog;
  NONDET_CALL *nondetLog;
  MEMORY_HEAP_ROW *heapLog;
  TRACK_BB_ROW *trackBBLog;
  switch (container.type) {
  case LIST_LOG_CONTAINER:
    listLogRows = (LIST_LOG_ROW *)container.values;
    return (&listLogRows[index]);
  case ALLOCATION_LOG_CONTAINER:
    allocationLog = (MEMORY_ALLOCATIONS_ROW *)container.values;
    return (&allocationLog[index]);
  case NONDET_LOG_CONTAINER:
    nondetLog = (NONDET_CALL *)container.values;
    return (&nondetLog[index]);
  case HEAP_LOG_CONTAINER:
    heapLog = (MEMORY_HEAP_ROW *)container.values;
    return (&heapLog[index]);
  case TRACKBB_LOG_CONTAINER:
    trackBBLog = (TRACK_BB_ROW *)container.values;
    return (&trackBBLog[index]);
  }
  return NULL;
}
