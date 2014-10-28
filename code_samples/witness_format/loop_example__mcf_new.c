#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include <stdio.h>                      /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/

#include <stdlib.h>

extern void __VERIFIER_error(int numline);

int main()
{
   int a = 10;

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(a), (void *)(intptr_t)(NULL), "a", 1, 0, 0, 0, "int", 6); /** by FORTES **/

   int c = 1 || (a = 0);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(c), (void *)(intptr_t)(NULL), "c", 1, 0, 0, 0, "int", 7); /** by FORTES **/

   int *p = 0;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(p), (void *)(intptr_t)(p), "p", 1, 0, 0, 0, "int", 8); /** by FORTES **/

   if (a)
   {
      while (c < a)
      {
         p            = &a;
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(p), (void *)(intptr_t)(p), "p", 1, 0, 0, 0, "int", 11); /** by FORTES **/

         c++;
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(c), (void *)(intptr_t)(c), "c", 1, 0, 0, 0, "int", 12); /** by FORTES **/
      }
      __VERIFIER_error(14);
      return(1);
   }
   return(0);
}
