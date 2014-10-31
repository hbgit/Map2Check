#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include <stdio.h>                      /** by FORTES **/
#include <stdlib.h>                     /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/


char *cstrcat(char *s1, const char *s2)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(s1), (void *)(intptr_t)(s1), "s1", 1, 0, 0, 0, "char", 1); /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(s2), (void *)(intptr_t)(s2), "s2", 1, 0, 0, 0, "char", 1); /** by FORTES **/

    print_debug(list_LOG_mcf);

   char *s = s1;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(s), (void *)(intptr_t)(s), "s", 1, 0, 0, 0, "char", 3); /** by FORTES **/
   
   print_debug(list_LOG_mcf);

// FORTES: Claim 1
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&(*(s)), (void *)(intptr_t)((s)))), "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 5 \n \t Comments:   dereference failure: invalid pointer");
   while ((*s) != '\0')
   {
      s++;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(s), (void *)(intptr_t)(s), "s", 1, 0, 0, 0, "char", 6); /** by FORTES **/
   }
// FORTES: Claim 2
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 2 \n \t Location at original code in line: 8 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 3
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 3 \n \t Location at original code in line: 8 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 4
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 4 \n \t Location at original code in line: 8 \n \t Comments:   dereference failure: invalid pointer");
   while ((*(s++) = *(s2++)) != '\0')
   {
      {
      }
   }
   return(s1);
}


int main()
{
   char *s1;
   char *s2;

   cstrcat(s1, s2);
   return(0);
}
