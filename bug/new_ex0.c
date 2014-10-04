#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include <stdio.h>                      /** by FORTES **/
#include <stdlib.h>                     /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/


extern void __VERIFIER_error(int numline);

int main()
{
   int a = 1;

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(a), (void *)(intptr_t)(NULL), 1, 0, 0, 0, "int", 4); /** by FORTES **/

   int c = 1 || (a = 0);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(c), (void *)(intptr_t)(NULL), 1, 0, 0, 0, "int", 5); /** by FORTES **/

   if (a)
   {
      __VERIFIER_error(6);
      return(1);
   }
   return(0);
}
