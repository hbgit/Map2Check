#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include <stdio.h>                      /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/

#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);

void test_fun(int a[], int N)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(a), (void *)(intptr_t)(NULL), "a", 1, 0, 0, 0, "int", 5); /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(N), (void *)(intptr_t)(NULL), "N", 1, 0, 0, 0, "int", 5); /** by FORTES **/

   int i;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(i), (void *)(intptr_t)(NULL), "i", 1, 0, 0, 0, "int", 7); /** by FORTES **/

   int pos = 0;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(pos), (void *)(intptr_t)(NULL), "pos", 1, 0, 0, 0, "int", 8); /** by FORTES **/

   int neg = 0;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(neg), (void *)(intptr_t)(NULL), "neg", 1, 0, 0, 0, "int", 9); /** by FORTES **/

   for (i = 0; i < N; i++)
   {
// FORTES: Claim 1
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&(i + a), (void *)(intptr_t)(i + a))), "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 11 \n \t Comments:   dereference failure: invalid pointer");
      while (a[i] < 0)
      {
// FORTES: Claim 2
         __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&(i + a), (void *)(intptr_t)(i + a))), "[Failed]\n VIOLATED PROPERTY: Claim 2 \n \t Location at original code in line: 12 \n \t Comments:   dereference failure: invalid pointer");
         a[i]++;
         neg++;
      }
// FORTES: Claim 3
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&(i + a), (void *)(intptr_t)(i + a))), "[Failed]\n VIOLATED PROPERTY: Claim 3 \n \t Location at original code in line: 15 \n \t Comments:   dereference failure: invalid pointer");
      while (a[i] > 0)
      {
// FORTES: Claim 4
         __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&(i + a), (void *)(intptr_t)(i + a))), "[Failed]\n VIOLATED PROPERTY: Claim 4 \n \t Location at original code in line: 16 \n \t Comments:   dereference failure: invalid pointer");
         a[i]--;
         pos++;
      }
   }
}


int main()
{
   int array_size = __VERIFIER_nondet_int();

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(array_size), (void *)(intptr_t)(NULL), "array_size", 2, 0, 0, 0, "int", 23); /** by FORTES **/

   if (array_size < 1)
   {
      array_size   = 1;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(array_size), (void *)(intptr_t)(NULL), "array_size", 2, 0, 0, 0, "int", 25); /** by FORTES **/
   }
   int *numbers = (int *)alloca(array_size * (sizeof(int)));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(numbers), (void *)(intptr_t)(numbers), "numbers", 2, 0, 0, 0, "int", 27); /** by FORTES **/

   test_fun(numbers, array_size);
}
