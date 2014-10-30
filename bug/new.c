#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include <stdio.h>                      /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/

#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);

int *x;

int foo(void)
{
// FORTES: Claim 1
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 8 \n \t Comments:   dereference failure: invalid pointer");
   (*x)--;
}


int main()
{
   //MAP_GB_VAR_FORTES(); /** by FORTES **/

   x            = alloca(sizeof(int));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(x), (void *)(intptr_t)(x), "x", 1, 0, 0, 0, "int", 12); /** by FORTES **/

// FORTES: Claim 2
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&(x), (void *)(intptr_t)(x))), "[Failed]\n VIOLATED PROPERTY: Claim 2 \n \t Location at original code in line: 13 \n \t Comments:   dereference failure: invalid pointer");
   *x           = __VERIFIER_nondet_int();
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(x), (void *)(intptr_t)(x), "x", 1, 0, 0, 0, "int", 13); /** by FORTES **/

// FORTES: Claim 3
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&(x), (void *)(intptr_t)(x))), "[Failed]\n VIOLATED PROPERTY: Claim 3 \n \t Location at original code in line: 15 \n \t Comments:   dereference failure: invalid pointer");
   while ((*x) > 0)
   {
      if (__VERIFIER_nondet_int())
      {
         foo();
      }
      else
      {
         foo();
      }
   }
   return(0);
}
