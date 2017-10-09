/**********************************************************************
 * This file contains a data structure to hold ListLogs and AllocationLogs
 ***********************************************************************/

#include "Map2CheckTypes.h"


#ifndef Container_H
#define Container_H

/**
 * Creates new container of specific type
 * @param type  Type of the Container
 * @return      The Container holding the values
 */ 
MAP2CHECK_CONTAINER new_container(enum Container_Type type);

/**
 * Releases all allocated resources of container
 * @param container  Pointer to the container
 * @return           A boolean representing success of operation
 */ 
Bool free_container(MAP2CHECK_CONTAINER* container);

/**
 * Adds new element to container
 * @param container  Pointer to the container
 * @param row        Pointer to the row to be added
 * @return           A boolean representing success of operation
 */ 
Bool append_element(MAP2CHECK_CONTAINER* container, void* row);

/**
 * Adds new element to container
 * @param index      Index to be accessed
 * @param container  Pointer to the container
 * @return           A pointer to the row
 */ 
void* get_element_at(unsigned index, MAP2CHECK_CONTAINER container);


#endif
