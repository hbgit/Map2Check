#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include <stdio.h>                      /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/

#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);

int main()
{
   int i;
   int j;

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(i), (void *)(intptr_t)(NULL), "i", 1, 0, 0, 0, "int", 5); /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(j), (void *)(intptr_t)(NULL), "j", 1, 0, 0, 0, "int", 5); /** by FORTES **/

   int length = __VERIFIER_nondet_int();
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(length), (void *)(intptr_t)(NULL), "length", 1, 0, 0, 0, "int", 6); /** by FORTES **/

   if (length < 1)
   {
      length = 1;
   }
   int *arr = alloca(length);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(arr), (void *)(intptr_t)(arr), "arr", 1, 0, 0, 0, "int", 8); /** by FORTES **/

   if (!arr)
   {
      return(0);
   }
   int *a = arr;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(a), (void *)(intptr_t)(a), "a", 1, 0, 0, 0, "int", 10); /** by FORTES **/

// FORTES: Claim 1
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((*a)), (void *)(intptr_t)((*a)))), "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 11 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 2
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 2 \n \t Location at original code in line: 11 \n \t Comments:   dereference failure: invalid pointer");
   while ((*a) != (*((arr + length) - 1)))
   {
// FORTES: Claim 3
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((a)), (void *)(intptr_t)((a)))), "[Failed]\n VIOLATED PROPERTY: Claim 3 \n \t Location at original code in line: 12 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 4
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 4 \n \t Location at original code in line: 12 \n \t Comments:   dereference failure: invalid pointer");
      *a          += *((arr + length) - 1);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(a), (void *)(intptr_t)(a), "a", 1, 0, 0, 0, "int", 12); /** by FORTES **/

      a++;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(a), (void *)(intptr_t)(a), "a", 1, 0, 0, 0, "int", 13); /** by FORTES **/
   }
   return(0);
}
