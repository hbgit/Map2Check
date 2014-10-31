#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include <stdio.h>                      /** by FORTES **/
#include <stdlib.h>                     /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/


void delete(char *x, char *y)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(x), (void *)(intptr_t)(x), "x", 1, 0, 0, 0, "char", 1); /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(y), (void *)(intptr_t)(y), "y", 1, 0, 0, 0, "char", 1); /** by FORTES **/
   print_debug(list_LOG_mcf);
// FORTES: Claim 1
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((y)), (void *)(intptr_t)((y)))), "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 2 \n \t Comments:   dereference failure: invalid pointer");
   *y           = 0;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(y), (void *)(intptr_t)(y), "y", 1, 0, 0, 0, "char", 2); /** by FORTES **/

// FORTES: Claim 2
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 2 \n \t Location at original code in line: 3 \n \t Comments:   same object violation");
// FORTES: Claim 3
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 3 \n \t Location at original code in line: 3 \n \t Comments:   same object violation");
   if ((0 < x) && (x < y))
   {
// FORTES: Claim 4
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((x)), (void *)(intptr_t)((x)))), "[Failed]\n VIOLATED PROPERTY: Claim 4 \n \t Location at original code in line: 4 \n \t Comments:   dereference failure: invalid pointer");
      while ((*x) != 0)
      {
// FORTES: Claim 5
         __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((x)), (void *)(intptr_t)((x)))), "[Failed]\n VIOLATED PROPERTY: Claim 5 \n \t Location at original code in line: 5 \n \t Comments:   dereference failure: invalid pointer");
         *x           = 0;
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(x), (void *)(intptr_t)(x), "x", 1, 0, 0, 0, "char", 5); /** by FORTES **/

         x++;
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(x), (void *)(intptr_t)(x), "x", 1, 0, 0, 0, "char", 6); /** by FORTES **/
      }
   }
}


int main()
{
   char *x;
   char *y;

   delete(x, y);
   return(0);
}
