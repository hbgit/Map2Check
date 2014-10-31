#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include <stdio.h>                      /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/

#include <stdlib.h>

extern void __VERIFIER_error(int linenumber);

extern int __VERIFIER_nondet_int(void);

int main()
{
   struct T
   {
      struct T *next;
      struct T *prev;
      int      data;
   };
   struct T2
   {
      struct T  head;
      struct T2 *next;
      struct T2 *prev;
   };
   struct T2 *first = 0;

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(first), (void *)(intptr_t)(first), "first", 1, 0, 0, 0, "", 21); /** by FORTES **/

   struct T2 *last = 0;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(last), (void *)(intptr_t)(last), "last", 1, 0, 0, 0, "", 22); /** by FORTES **/

   while (__VERIFIER_nondet_int())
   {
      struct T2 *x = malloc(sizeof(struct T2));
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(x), (void *)(intptr_t)(x), "x", 1, 1, 0, 0, "", 26); /** by FORTES **/

      if (!x)
      {
         assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 28)); /** by FORTES **/
         abort();
      }
      x->next      = 0;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(x->next), (void *)(intptr_t)(x->next), "x->next", 1, 0, 0, 0, "", 30); /** by FORTES **/

      x->prev      = 0;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(x->prev), (void *)(intptr_t)(x->prev), "x->prev", 1, 0, 0, 0, "", 31); /** by FORTES **/

      x->head.next = &x->head;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(x->head.next), (void *)(intptr_t)(x->head.next), "x->head.next", 1, 0, 0, 0, "", 32); /** by FORTES **/

      x->head.prev = &x->head;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(x->head.prev), (void *)(intptr_t)(x->head.prev), "x->head.prev", 1, 0, 0, 0, "", 33); /** by FORTES **/

      x->head.data = 0;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(x->head.data), (void *)(intptr_t)(x->head.data), "x->head.data", 1, 0, 0, 0, "", 34); /** by FORTES **/

      struct T *y = 0;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(y), (void *)(intptr_t)(y), "y", 1, 0, 0, 0, "", 36); /** by FORTES **/

      while (__VERIFIER_nondet_int())
      {
         y            = malloc(sizeof(struct T));
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(y), (void *)(intptr_t)(y), "y", 1, 1, 0, 0, "", 39); /** by FORTES **/

         if (!y)
         {
            assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 41)); /** by FORTES **/
            abort();
         }
         y->next      = x->head.next;
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(y->next), (void *)(intptr_t)(y->next), "y->next", 1, 0, 0, 0, "", 43); /** by FORTES **/

         y->next->prev = y;
         list_LOG_mcf  = mark_map_FORTES(list_LOG_mcf, (void *)&(y->next->prev), (void *)(intptr_t)(y->next->prev), "y->next->prev", 1, 0, 0, 0, "", 44); /** by FORTES **/

         y->prev      = &x->head;
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(y->prev), (void *)(intptr_t)(y->prev), "y->prev", 1, 0, 0, 0, "", 45); /** by FORTES **/

         y->data      = 0;
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(y->data), (void *)(intptr_t)(y->data), "y->data", 1, 0, 0, 0, "", 46); /** by FORTES **/

         x->head.next = y;
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(x->head.next), (void *)(intptr_t)(x->head.next), "x->head.next", 1, 0, 0, 0, "", 47); /** by FORTES **/

         y            = 0;
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(y), (void *)(intptr_t)(y), "y", 1, 0, 0, 0, "", 48); /** by FORTES **/
      }
      if (!first)
      {
         first        = x;
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(first), (void *)(intptr_t)(first), "first", 1, 0, 0, 0, "", 52); /** by FORTES **/

         last         = x;
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(last), (void *)(intptr_t)(last), "last", 1, 0, 0, 0, "", 53); /** by FORTES **/
      }
      else
      {
// FORTES: Claim 1
         __MAP_FORTES_assert(!(last == NULL), "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 55 \n \t Comments:   dereference failure: NULL pointer");
         last->next   = x;
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(last->next), (void *)(intptr_t)(last->next), "last->next", 1, 0, 0, 0, "", 55); /** by FORTES **/

         x->prev      = last;
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(x->prev), (void *)(intptr_t)(x->prev), "x->prev", 1, 0, 0, 0, "", 56); /** by FORTES **/

         last         = x;
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(last), (void *)(intptr_t)(last), "last", 1, 0, 0, 0, "", 57); /** by FORTES **/
      }
   }
   while (first)
   {
      struct T2 *x = first;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(x), (void *)(intptr_t)(x), "x", 1, 0, 0, 0, "", 64); /** by FORTES **/

// FORTES: Claim 2
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((first)), (void *)(intptr_t)((first)))), "[Failed]\n VIOLATED PROPERTY: Claim 2 \n \t Location at original code in line: 65 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 3
      __MAP_FORTES_assert(!(first == NULL), "[Failed]\n VIOLATED PROPERTY: Claim 3 \n \t Location at original code in line: 65 \n \t Comments:   dereference failure: NULL pointer");
// FORTES: Claim 4
      __MAP_FORTES_assert(!((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&first, (void *)(intptr_t)first))) || !(IS_VALID_OBJECT_FORTES(list_LOG_mcf, (void *)&((first)), (void *)(intptr_t)((first)))), "[Failed]\n VIOLATED PROPERTY: Claim 4 \n \t Location at original code in line: 65 \n \t Comments:   dereference failure: invalidated dynamic object");
      first        = first->next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(first), (void *)(intptr_t)(first), "first", 1, 0, 0, 0, "", 65); /** by FORTES **/

// FORTES: Claim 5
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((x)), (void *)(intptr_t)((x)))), "[Failed]\n VIOLATED PROPERTY: Claim 5 \n \t Location at original code in line: 67 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 6
      __MAP_FORTES_assert(!(x == NULL), "[Failed]\n VIOLATED PROPERTY: Claim 6 \n \t Location at original code in line: 67 \n \t Comments:   dereference failure: NULL pointer");
// FORTES: Claim 7
      __MAP_FORTES_assert(!((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&x, (void *)(intptr_t)x))) || !(IS_VALID_OBJECT_FORTES(list_LOG_mcf, (void *)&((x)), (void *)(intptr_t)((x)))), "[Failed]\n VIOLATED PROPERTY: Claim 7 \n \t Location at original code in line: 67 \n \t Comments:   dereference failure: invalidated dynamic object");
      struct T *y = x->head.next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(y), (void *)(intptr_t)(y), "y", 1, 0, 0, 0, "", 67); /** by FORTES **/

// FORTES: Claim 8
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((x)), (void *)(intptr_t)((x)))), "[Failed]\n VIOLATED PROPERTY: Claim 8 \n \t Location at original code in line: 68 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 9
      __MAP_FORTES_assert(!(x == NULL), "[Failed]\n VIOLATED PROPERTY: Claim 9 \n \t Location at original code in line: 68 \n \t Comments:   dereference failure: NULL pointer");
// FORTES: Claim 10
      __MAP_FORTES_assert(!((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&x, (void *)(intptr_t)x))) || !(IS_VALID_OBJECT_FORTES(list_LOG_mcf, (void *)&((x)), (void *)(intptr_t)((x)))), "[Failed]\n VIOLATED PROPERTY: Claim 10 \n \t Location at original code in line: 68 \n \t Comments:   dereference failure: invalidated dynamic object");
      while (y != (&x->head))
      {
         struct T *z = y;
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(z), (void *)(intptr_t)(z), "z", 1, 0, 0, 0, "", 69); /** by FORTES **/

// FORTES: Claim 11
         __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((y)), (void *)(intptr_t)((y)))), "[Failed]\n VIOLATED PROPERTY: Claim 11 \n \t Location at original code in line: 70 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 12
         __MAP_FORTES_assert(!((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&y, (void *)(intptr_t)y))) || !(IS_VALID_OBJECT_FORTES(list_LOG_mcf, (void *)&((y)), (void *)(intptr_t)((y)))), "[Failed]\n VIOLATED PROPERTY: Claim 12 \n \t Location at original code in line: 70 \n \t Comments:   dereference failure: invalidated dynamic object");
         y            = y->next;
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(y), (void *)(intptr_t)(y), "y", 1, 0, 0, 0, "", 70); /** by FORTES **/

// FORTES: Claim 13
         __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((z)), (void *)(intptr_t)((z)))), "[Failed]\n VIOLATED PROPERTY: Claim 13 \n \t Location at original code in line: 71 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 14
         __MAP_FORTES_assert(!((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&z, (void *)(intptr_t)z))) || !(IS_VALID_OBJECT_FORTES(list_LOG_mcf, (void *)&((z)), (void *)(intptr_t)((z)))), "[Failed]\n VIOLATED PROPERTY: Claim 14 \n \t Location at original code in line: 71 \n \t Comments:   dereference failure: invalidated dynamic object");
         assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(z), 71));                                              /** by FORTES **/
         free(z);
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(z), (void *)(intptr_t)(z), "z", 1, 0, 1, 0, "", 71); /** by FORTES **/
      }
// FORTES: Claim 15
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((x)), (void *)(intptr_t)((x)))), "[Failed]\n VIOLATED PROPERTY: Claim 15 \n \t Location at original code in line: 74 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 16
      __MAP_FORTES_assert(!(x == NULL), "[Failed]\n VIOLATED PROPERTY: Claim 16 \n \t Location at original code in line: 74 \n \t Comments:   dereference failure: NULL pointer");
// FORTES: Claim 17
      __MAP_FORTES_assert(!((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&x, (void *)(intptr_t)x))) || !(IS_VALID_OBJECT_FORTES(list_LOG_mcf, (void *)&((x)), (void *)(intptr_t)((x)))), "[Failed]\n VIOLATED PROPERTY: Claim 17 \n \t Location at original code in line: 74 \n \t Comments:   dereference failure: invalidated dynamic object");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(x), 74));                                              /** by FORTES **/
      free(x);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(x), (void *)(intptr_t)(x), "x", 1, 0, 1, 0, "", 74); /** by FORTES **/
   }
   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 78));                                                                /** by FORTES **/
   return(0);

   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 78)); /** by FORTES **/
}
