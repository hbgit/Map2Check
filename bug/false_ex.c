#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/

#include <stdio.h>
#include <stdlib.h>

int main()
{
   int *A = malloc(10);

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(A), (void *)(intptr_t)(A), "A", 1, 1, 0, 0, "int", 6); /** by FORTES **/

   int *B = malloc(10);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(B), (void *)(intptr_t)(B), "B", 1, 1, 0, 0, "int", 7); /** by FORTES **/

   B            = A;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(B), (void *)(intptr_t)(B), "B", 1, 0, 0, 0, "int", 9);  /** by FORTES **/

   assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(A), 11));                                                 /** by FORTES **/
   free(A);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(A), (void *)(intptr_t)(A), "A", 1, 0, 1, 0, "int", 11); /** by FORTES **/
	
	
// FORTES: Claim 1
   __MAP_FORTES_assert(!((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&B, (void *)(intptr_t)B))) || !(IS_VALID_OBJECT_FORTES(list_LOG_mcf, (void *)&(B), (void *)(intptr_t)(B))), "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 12 \n \t Comments:   dereference failure: invalidated dynamic object");
   assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(B), 12));                                                 /** by FORTES **/
   free(B);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(B), (void *)(intptr_t)(B), "B", 1, 0, 1, 0, "int", 12); /** by FORTES **/
   
   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 4));                                                                 /** by FORTES **/
}
