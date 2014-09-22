#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include <stdio.h>                      /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/

#include <stdlib.h>

extern void __VERIFIER_error();

void exit(int s)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(s), (void *)(intptr_t)(NULL), 1, 0, 0, 0, 10); /** by FORTES **/

_EXIT:
   goto _EXIT;
}


typedef struct node
{
   int         h;
   struct node *n;
} *List;

int main()
{
   List a = (List)malloc(sizeof(struct node));

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(a), (void *)(intptr_t)(a), 2, 1, 0, 0, 21); /** by FORTES **/

   if (a == 0)
   {
      assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 29)); /** by FORTES **/
      exit(1);
   }
   List t;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(t), (void *)(intptr_t)(NULL), 2, 0, 0, 0, 25); /** by FORTES **/

   List p = a;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(p), (void *)(intptr_t)(p), 2, 0, 0, 0, 26); /** by FORTES **/

   int i = 0;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(i), (void *)(intptr_t)(NULL), 2, 0, 0, 0, 28); /** by FORTES **/

   while ((i < 30) && __VERIFIER_nondet_int())
   {
      p->h         = i;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(p->h), (void *)(intptr_t)(p->h), 2, 0, 0, 0, 31); /** by FORTES **/

      t            = (List)malloc(sizeof(struct node));
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(t), (void *)(intptr_t)(t), 2, 1, 0, 0, 32); /** by FORTES **/

      if (t == 0)
      {
         assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 44)); /** by FORTES **/
         exit(1);
      }
      p->n         = t;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(p->n), (void *)(intptr_t)(p->n), 2, 0, 0, 0, 36); /** by FORTES **/

      p            = p->n;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(p), (void *)(intptr_t)(p), 2, 0, 0, 0, 37); /** by FORTES **/

      i++;
   }
   p->h         = i;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(p->h), (void *)(intptr_t)(p->h), 2, 0, 0, 0, 41); /** by FORTES **/

   p->n         = 0;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(p->n), (void *)(intptr_t)(p->n), 2, 0, 0, 0, 42); /** by FORTES **/

   p            = a;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(p), (void *)(intptr_t)(p), 2, 0, 0, 0, 43); /** by FORTES **/

   i = 0;
   while (p != 0)
   {
// FORTES: Claim 1
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&(p), (void *)(intptr_t)(p))), "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 46 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 2
      __MAP_FORTES_assert(!(p == NULL), "[Failed]\n VIOLATED PROPERTY: Claim 2 \n \t Location at original code in line: 46 \n \t Comments:   dereference failure: NULL pointer");
      if (p->h != 1)
      {
ERROR:
         __VERIFIER_error();
      }
// FORTES: Claim 3
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&(p), (void *)(intptr_t)(p))), "[Failed]\n VIOLATED PROPERTY: Claim 3 \n \t Location at original code in line: 49 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 4
      __MAP_FORTES_assert(!(p == NULL), "[Failed]\n VIOLATED PROPERTY: Claim 4 \n \t Location at original code in line: 49 \n \t Comments:   dereference failure: NULL pointer");
      p            = p->n;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(p), (void *)(intptr_t)(p), 2, 0, 0, 0, 49); /** by FORTES **/

      i++;
   }
   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 65)); /** by FORTES **/
   return(0);

   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 22)); /** by FORTES **/
}
