#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include <stdio.h>                      /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/

#include <stdlib.h>
#include <string.h>

extern void __VERIFIER_error(int linenumber);

static void *calloc_model(size_t nmemb, size_t size)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(nmemb), (void *)(intptr_t)(NULL), "nmemb", 1, 0, 0, 0, "int", 6); /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(size), (void *)(intptr_t)(NULL), "size", 1, 0, 0, 0, "int", 6);   /** by FORTES **/

   void *ptr = malloc(nmemb * size);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(ptr), (void *)(intptr_t)(ptr), "ptr", 1, 1, 0, 0, "void", 7); /** by FORTES **/

   return(memset(ptr, 0, nmemb * size));
}


extern int __VERIFIER_nondet_int(void);

struct L4
{
   struct L3 **owner;
   struct L4 *next;
   struct L5 *down;
};

struct L3
{
   struct L4 *down;
   struct L3 *next;
   struct L3 **owner;
};

struct L2
{
   struct L1 **owner;
   struct L2 *next;
   struct L3 *down;
};

struct L1
{
   struct L2 *down;
   struct L1 *next;
   struct L1 **owner;
};

struct L0
{
   struct L0 *next;
   struct L1 *down;
};

static void *zalloc_or_die(unsigned size)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(size), (void *)(intptr_t)(NULL), "size", 2, 0, 0, 0, "unsigned", 42); /** by FORTES **/

   void *ptr = calloc_model(1U, size);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(ptr), (void *)(intptr_t)(ptr), "ptr", 2, 0, 0, 0, "void", 44); /** by FORTES **/

   if (ptr)
   {
      return(ptr);
   }
   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 48)); /** by FORTES **/
   abort();
}


static void l4_insert(struct L4 **list, void *owner)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 3, 0, 0, 0, "", 51);        /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(owner), (void *)(intptr_t)(owner), "owner", 3, 0, 0, 0, "void", 51); /** by FORTES **/

   struct L4 *item = zalloc_or_die(sizeof(*item));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item), (void *)(intptr_t)(item), "item", 3, 0, 0, 0, "", 53); /** by FORTES **/

// FORTES: Claim 1
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)((item)))), "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 54 \n \t Comments:   dereference failure: invalid pointer");
   item->down   = zalloc_or_die(119U);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item->down), (void *)(intptr_t)(item->down), "item->down", 3, 0, 0, 0, "", 54); /** by FORTES **/

// FORTES: Claim 2
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)((item)))), "[Failed]\n VIOLATED PROPERTY: Claim 2 \n \t Location at original code in line: 55 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 3
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 3 \n \t Location at original code in line: 55 \n \t Comments:   dereference failure: invalid pointer");
   *((void **)item->down) = item;
   list_LOG_mcf           = mark_map_FORTES(list_LOG_mcf, (void *)&(*((void **)item->down)), (void *)(intptr_t)(*((void **)item->down)), "*((void **) item->down)", 3, 0, 0, 0, "", 55); /** by FORTES **/

// FORTES: Claim 4
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)((item)))), "[Failed]\n VIOLATED PROPERTY: Claim 4 \n \t Location at original code in line: 57 \n \t Comments:   dereference failure: invalid pointer");
   item->owner  = owner;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item->owner), (void *)(intptr_t)(item->owner), "item->owner", 3, 0, 0, 0, "", 57); /** by FORTES **/

// FORTES: Claim 5
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)((item)))), "[Failed]\n VIOLATED PROPERTY: Claim 5 \n \t Location at original code in line: 58 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 6
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 6 \n \t Location at original code in line: 58 \n \t Comments:   dereference failure: invalid pointer");
   item->next   = *list;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item->next), (void *)(intptr_t)(item->next), "item->next", 3, 0, 0, 0, "", 58); /** by FORTES **/

// FORTES: Claim 7
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 7 \n \t Location at original code in line: 59 \n \t Comments:   dereference failure: invalid pointer");
   *list        = item;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 3, 0, 0, 0, "", 59); /** by FORTES **/
}


static void l3_insert(struct L3 **list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 4, 0, 0, 0, "", 62); /** by FORTES **/

   struct L3 *item = zalloc_or_die(sizeof(*item));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item), (void *)(intptr_t)(item), "item", 4, 0, 0, 0, "", 64); /** by FORTES **/

   do
   {
// FORTES: Claim 8
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)((item)))), "[Failed]\n VIOLATED PROPERTY: Claim 8 \n \t Location at original code in line: 67 \n \t Comments:   dereference failure: invalid pointer");
      l4_insert(&item->down, list);
   } while (__VERIFIER_nondet_int());
// FORTES: Claim 9
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)((item)))), "[Failed]\n VIOLATED PROPERTY: Claim 9 \n \t Location at original code in line: 70 \n \t Comments:   dereference failure: invalid pointer");
   item->owner  = list;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item->owner), (void *)(intptr_t)(item->owner), "item->owner", 4, 0, 0, 0, "", 70); /** by FORTES **/

// FORTES: Claim 10
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)((item)))), "[Failed]\n VIOLATED PROPERTY: Claim 10 \n \t Location at original code in line: 71 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 11
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 11 \n \t Location at original code in line: 71 \n \t Comments:   dereference failure: invalid pointer");
   item->next   = *list;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item->next), (void *)(intptr_t)(item->next), "item->next", 4, 0, 0, 0, "", 71); /** by FORTES **/

// FORTES: Claim 12
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 12 \n \t Location at original code in line: 72 \n \t Comments:   dereference failure: invalid pointer");
   *list        = item;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 4, 0, 0, 0, "", 72); /** by FORTES **/
}


static void l2_insert(struct L2 **list, void *owner)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 5, 0, 0, 0, "", 75);        /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(owner), (void *)(intptr_t)(owner), "owner", 5, 0, 0, 0, "void", 75); /** by FORTES **/

   struct L2 *item = zalloc_or_die(sizeof(*item));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item), (void *)(intptr_t)(item), "item", 5, 0, 0, 0, "", 77); /** by FORTES **/

   do
   {
// FORTES: Claim 13
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)((item)))), "[Failed]\n VIOLATED PROPERTY: Claim 13 \n \t Location at original code in line: 80 \n \t Comments:   dereference failure: invalid pointer");
      l3_insert(&item->down);
   } while (__VERIFIER_nondet_int());
// FORTES: Claim 14
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)((item)))), "[Failed]\n VIOLATED PROPERTY: Claim 14 \n \t Location at original code in line: 83 \n \t Comments:   dereference failure: invalid pointer");
   item->owner  = owner;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item->owner), (void *)(intptr_t)(item->owner), "item->owner", 5, 0, 0, 0, "", 83); /** by FORTES **/

// FORTES: Claim 15
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)((item)))), "[Failed]\n VIOLATED PROPERTY: Claim 15 \n \t Location at original code in line: 84 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 16
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 16 \n \t Location at original code in line: 84 \n \t Comments:   dereference failure: invalid pointer");
   item->next   = *list;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item->next), (void *)(intptr_t)(item->next), "item->next", 5, 0, 0, 0, "", 84); /** by FORTES **/

// FORTES: Claim 17
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 17 \n \t Location at original code in line: 85 \n \t Comments:   dereference failure: invalid pointer");
   *list        = item;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 5, 0, 0, 0, "", 85); /** by FORTES **/
}


static void l1_insert(struct L1 **list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 6, 0, 0, 0, "", 88); /** by FORTES **/

   struct L1 *item = zalloc_or_die(sizeof(*item));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item), (void *)(intptr_t)(item), "item", 6, 0, 0, 0, "", 90); /** by FORTES **/

   do
   {
// FORTES: Claim 18
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)((item)))), "[Failed]\n VIOLATED PROPERTY: Claim 18 \n \t Location at original code in line: 93 \n \t Comments:   dereference failure: invalid pointer");
      l2_insert(&item->down, list);
   } while (__VERIFIER_nondet_int());
// FORTES: Claim 19
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)((item)))), "[Failed]\n VIOLATED PROPERTY: Claim 19 \n \t Location at original code in line: 96 \n \t Comments:   dereference failure: invalid pointer");
   item->owner  = list;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item->owner), (void *)(intptr_t)(item->owner), "item->owner", 6, 0, 0, 0, "", 96); /** by FORTES **/

// FORTES: Claim 20
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)((item)))), "[Failed]\n VIOLATED PROPERTY: Claim 20 \n \t Location at original code in line: 97 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 21
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 21 \n \t Location at original code in line: 97 \n \t Comments:   dereference failure: invalid pointer");
   item->next   = *list;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item->next), (void *)(intptr_t)(item->next), "item->next", 6, 0, 0, 0, "", 97); /** by FORTES **/

// FORTES: Claim 22
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 22 \n \t Location at original code in line: 98 \n \t Comments:   dereference failure: invalid pointer");
   *list        = item;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 6, 0, 0, 0, "", 98); /** by FORTES **/
}


static void l0_insert(struct L0 **list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 7, 0, 0, 0, "", 101); /** by FORTES **/

   struct L0 *item = zalloc_or_die(sizeof(*item));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item), (void *)(intptr_t)(item), "item", 7, 0, 0, 0, "", 103); /** by FORTES **/

   do
   {
// FORTES: Claim 23
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)((item)))), "[Failed]\n VIOLATED PROPERTY: Claim 23 \n \t Location at original code in line: 106 \n \t Comments:   dereference failure: invalid pointer");
      l1_insert(&item->down);
   } while (__VERIFIER_nondet_int());
// FORTES: Claim 24
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)((item)))), "[Failed]\n VIOLATED PROPERTY: Claim 24 \n \t Location at original code in line: 109 \n \t Comments:   dereference failure: invalid pointer");
   item->next   = *list;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item->next), (void *)(intptr_t)(item->next), "item->next", 7, 0, 0, 0, "", 109); /** by FORTES **/

   *list        = item;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 7, 0, 0, 0, "", 110); /** by FORTES **/
}


static void l4_destroy(struct L4 *list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 8, 0, 0, 0, "", 113); /** by FORTES **/

   do
   {
// FORTES: Claim 25
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list->down)), (void *)(intptr_t)((list->down)))), "[Failed]\n VIOLATED PROPERTY: Claim 25 \n \t Location at original code in line: 116 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 26
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 26 \n \t Location at original code in line: 116 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 27
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 27 \n \t Location at original code in line: 116 \n \t Comments:   dereference failure: invalid pointer");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(list->down), 116));                                                                /** by FORTES **/
      free(list->down);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list->down), (void *)(intptr_t)(list->down), "list->down", 8, 0, 1, 0, "", 116); /** by FORTES **/

// FORTES: Claim 28
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 28 \n \t Location at original code in line: 118 \n \t Comments:   dereference failure: invalid pointer");
      struct L4 *next = list->next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(next), (void *)(intptr_t)(next), "next", 8, 0, 0, 0, "", 118); /** by FORTES **/

// FORTES: Claim 29
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 29 \n \t Location at original code in line: 119 \n \t Comments:   dereference failure: invalid pointer");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(list), 119));                                                    /** by FORTES **/
      free(list);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 8, 0, 1, 0, "", 119); /** by FORTES **/

      list         = next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 8, 0, 0, 0, "", 120); /** by FORTES **/
   } while (list);
}


static void l3_destroy(struct L3 *list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 9, 0, 0, 0, "", 125); /** by FORTES **/

   do
   {
// FORTES: Claim 30
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 30 \n \t Location at original code in line: 128 \n \t Comments:   dereference failure: invalid pointer");
      l4_destroy(list->down);
// FORTES: Claim 31
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 31 \n \t Location at original code in line: 130 \n \t Comments:   dereference failure: invalid pointer");
      struct L3 *next = list->next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(next), (void *)(intptr_t)(next), "next", 9, 0, 0, 0, "", 130); /** by FORTES **/

// FORTES: Claim 32
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 32 \n \t Location at original code in line: 131 \n \t Comments:   dereference failure: invalid pointer");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(list), 131));                                                    /** by FORTES **/
      free(list);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 9, 0, 1, 0, "", 131); /** by FORTES **/

      list         = next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 9, 0, 0, 0, "", 132); /** by FORTES **/
   } while (list);
}


static void l2_destroy(struct L2 *list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 10, 0, 0, 0, "", 137); /** by FORTES **/

   do
   {
// FORTES: Claim 33
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 33 \n \t Location at original code in line: 140 \n \t Comments:   dereference failure: invalid pointer");
      l3_destroy(list->down);
// FORTES: Claim 34
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 34 \n \t Location at original code in line: 142 \n \t Comments:   dereference failure: invalid pointer");
      struct L2 *next = list->next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(next), (void *)(intptr_t)(next), "next", 10, 0, 0, 0, "", 142); /** by FORTES **/

// FORTES: Claim 35
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 35 \n \t Location at original code in line: 143 \n \t Comments:   dereference failure: invalid pointer");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(list), 143));                                                     /** by FORTES **/
      free(list);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 10, 0, 1, 0, "", 143); /** by FORTES **/

      list         = next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 10, 0, 0, 0, "", 144); /** by FORTES **/
   } while (list);
}


static void l1_destroy(struct L1 *list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 11, 0, 0, 0, "", 149); /** by FORTES **/

   do
   {
// FORTES: Claim 36
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 36 \n \t Location at original code in line: 152 \n \t Comments:   dereference failure: invalid pointer");
      l2_destroy(list->down);
// FORTES: Claim 37
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 37 \n \t Location at original code in line: 154 \n \t Comments:   dereference failure: invalid pointer");
      struct L1 *next = list->next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(next), (void *)(intptr_t)(next), "next", 11, 0, 0, 0, "", 154); /** by FORTES **/

// FORTES: Claim 38
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 38 \n \t Location at original code in line: 155 \n \t Comments:   dereference failure: invalid pointer");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(list), 155));                                                     /** by FORTES **/
      free(list);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 11, 0, 1, 0, "", 155); /** by FORTES **/

      list         = next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 11, 0, 0, 0, "", 156); /** by FORTES **/
   } while (list);
}


static void l0_destroy(struct L0 *list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 12, 0, 0, 0, "", 161); /** by FORTES **/

   do
   {
// FORTES: Claim 39
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 39 \n \t Location at original code in line: 164 \n \t Comments:   dereference failure: NULL pointer");
// FORTES: Claim 40
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 40 \n \t Location at original code in line: 164 \n \t Comments:   dereference failure: invalid pointer");
      l1_destroy(list->down);
// FORTES: Claim 41
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 41 \n \t Location at original code in line: 166 \n \t Comments:   dereference failure: NULL pointer");
// FORTES: Claim 42
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 42 \n \t Location at original code in line: 166 \n \t Comments:   dereference failure: invalid pointer");
      struct L0 *next = list->next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(next), (void *)(intptr_t)(next), "next", 12, 0, 0, 0, "", 166); /** by FORTES **/

// FORTES: Claim 43
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 43 \n \t Location at original code in line: 167 \n \t Comments:   dereference failure: NULL pointer");
// FORTES: Claim 44
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 44 \n \t Location at original code in line: 167 \n \t Comments:   dereference failure: invalid pointer");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(list), 167));                                                     /** by FORTES **/
      free(list);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 12, 0, 1, 0, "", 167); /** by FORTES **/

      list         = next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 12, 0, 0, 0, "", 168); /** by FORTES **/
   } while (list);
}


int main()
{
   static struct L0 *list;

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(NULL), "list", 13, 0, 0, 0, "", 175); /** by FORTES **/

   do
   {
      l0_insert(&list);
   } while (__VERIFIER_nondet_int());
   l0_destroy(list);
   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 181)); /** by FORTES **/
}
