#ifndef CHECK_SAFETY_MEM_H_INCLUDED
#define CHECK_SAFETY_MEM_H_INCLUDED

#include <errno.h>
//#include <string.h>
#include <assert.h>
#include <limits.h>

#if !defined(NULL)
    #define NULL ((void*)0)
#endif


#define log_error(M, ...) fprintf(stderr, "\n " M "\n \n", __FILE__, __LINE__, ##__VA_ARGS__)
#define __MAP_FORTES_assert(A, M, ...) if(!(A)) {PRINT_TRACE_LOG(); log_error(M, ##__VA_ARGS__); assert(A); } //Update to print the trace


typedef struct obj {
  void  *adresse_mem_map_FORTES;
  void  *block_MEM_Point_FORTES;
  int   map_ID_func_FORTES;
  int   map_is_DYNAM_FORTES; 
  
  struct obj *next_item_FORTES; 
} LIST_DYN_OBJ_FORTES;


LIST_DYN_OBJ_FORTES* mark_map_FORTES(LIST_DYN_OBJ_FORTES* list, void *adress, void *block, char *namevar, int ID,
                                    int status, int setFree, int isUnion, char *type, int linePreCode);

int CHECK_MEMORY_LEAK(LIST_DYN_OBJ_FORTES* list, int ID_FUNC, int numLineCCode);

int IS_DYNAMIC_OBJECT_FORTES(LIST_DYN_OBJ_FORTES* list, void *adress, void *block);

int IS_VALID_OBJECT_FORTES(LIST_DYN_OBJ_FORTES* list, void *adress, void *block);

int IS_VALID_POINTER_FORTES(LIST_DYN_OBJ_FORTES* list, void *adress, void *block);

int INVALID_FREE(LIST_DYN_OBJ_FORTES* list, void *block, int numLineCCode);

void print_debug(LIST_DYN_OBJ_FORTES *list);

void PRINT_TRACE_LOG();

LIST_DYN_OBJ_FORTES *list_LOG_mcf=NULL;

int invalidObjectFortes = INT_MAX;

int __VERIFIER_nondet_int(void);

int CHECK_OVERWRITE_ADDR(void *actual_block, void *last_block, int numLineCCode);

void * GET_LAST_ADDR_POINTS_TO(LIST_DYN_OBJ_FORTES* list, void *adress);

void * GET_LAST_ADDR_FROM_MALLOC(LIST_DYN_OBJ_FORTES* list, void *adress);

void __VERIFIER_error(int numline);


#endif
