/**********************************************************************
* This file contains all methods that are used for the KleeLog which  *
* is a structure that contains all klee operations
***********************************************************************/

#include "Map2CheckTypes.h"
#include "Map2CheckGlobals.h"
#include "Container.h"

#ifndef KleeLog_H
#define KleeLog_H


/**
 * Generates file containing Klee Log
 * @param klee_container  Pointer to Container
 * @return                A bool representing success of operation
 */ 
Bool klee_log_to_file(MAP2CHECK_CONTAINER klee_container);

KLEE_CALL new_klee_call(enum NONDET_TYPE type, unsigned line, unsigned scope, void* value, const char* function_name, unsigned step);


#endif