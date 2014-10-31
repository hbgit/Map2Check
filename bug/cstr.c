#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include <stdio.h>                      /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/

#include <stdlib.h>
#include <string.h>

extern void __VERIFIER_error(int linenumber);

char a[sizeof(int *)];

void foo(void)
{
   int *p = (int *)malloc(10);

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(p), (void *)(intptr_t)(p), "p", 1, 1, 0, 0, "int", 13); /** by FORTES **/

   memcpy(a, &p, sizeof(p));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(memcpy), (void *)(intptr_t)(memcpy(a, &p, sizeof(p))), "memcpy", 1, 0, 0, 0, "int", 14); /** by FORTES **/
}


int main(void)
{
   MAP_GB_VAR_FORTES(); /** by FORTES **/

   foo();
   void *p;
   memcpy(&p, a, sizeof(p));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(memcpy), (void *)(intptr_t)(memcpy(&p, a, sizeof(p))), "memcpy", 2, 0, 0, 0, "void", 21); /** by FORTES **/

// FORTES: Claim 1
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((p)), (void *)(intptr_t)((p)))), "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 22 \n \t Comments:   dereference failure: invalid pointer");
   assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(p), 22));                                                  /** by FORTES **/
   free(p);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(p), (void *)(intptr_t)(p), "p", 2, 0, 1, 0, "void", 22); /** by FORTES **/

   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 22));                                                                 /** by FORTES **/
}


/** Mapping global variables <- by FORTES**/
MAP_GB_VAR_FORTES()
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(a), (void *)(intptr_t)(NULL), "a", 0, 0, 0, 0, "char", 9); /** by FORTES **/
}
/** End mapping global variables <- by FORTES**/
