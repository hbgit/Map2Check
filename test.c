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
   struct L4 *next;
   struct L5 *down;
};

struct L3
{
   struct L4 *down;
   struct L3 *next;
};

struct L2
{
   struct L2 *next;
   struct L3 *down;
};

struct L1
{
   struct L2 *down;
   struct L1 *next;
};

struct L0
{
   struct L0 *next;
   struct L1 *down;
};

static void *zalloc_or_die(unsigned size)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(size), (void *)(intptr_t)(NULL), "size", 2, 0, 0, 0, "unsigned", 38); /** by FORTES **/

   void *ptr = calloc_model(1U, size);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(ptr), (void *)(intptr_t)(ptr), "ptr", 2, 0, 0, 0, "void", 40); /** by FORTES **/

   if (ptr)
   {
      return(ptr);
   }
   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 44)); /** by FORTES **/
   abort();
}


static void l4_insert(struct L4 **list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 3, 0, 0, 0, "", 47); /** by FORTES **/

   struct L4 *item = zalloc_or_die(sizeof(*item));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item), (void *)(intptr_t)(item), "item", 3, 0, 0, 0, "", 49); /** by FORTES **/

// FORTES: Claim 1
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)(NULL))), "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 50 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
   item->down   = zalloc_or_die(119U);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item->down), (void *)(intptr_t)(item->down), "item->down", 3, 0, 0, 0, "", 50); /** by FORTES **/

// FORTES: Claim 2
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)(NULL))), "[Failed]\n VIOLATED PROPERTY: Claim 2 \n \t Location at original code in line: 52 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 3
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 3 \n \t Location at original code in line: 52 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
   item->next   = *list;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item->next), (void *)(intptr_t)(item->next), "item->next", 3, 0, 0, 0, "", 52); /** by FORTES **/

// FORTES: Claim 4
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 4 \n \t Location at original code in line: 53 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
   *list        = item;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(NULL), "list", 3, 0, 0, 0, "", 53); /** by FORTES **/
}


static void l3_insert(struct L3 **list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 4, 0, 0, 0, "", 56); /** by FORTES **/

   struct L3 *item = zalloc_or_die(sizeof(*item));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item), (void *)(intptr_t)(item), "item", 4, 0, 0, 0, "", 58); /** by FORTES **/

   do
   {
// FORTES: Claim 5
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)(NULL))), "[Failed]\n VIOLATED PROPERTY: Claim 5 \n \t Location at original code in line: 61 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      l4_insert(&item->down);
   } while (__VERIFIER_nondet_int());
// FORTES: Claim 6
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)(NULL))), "[Failed]\n VIOLATED PROPERTY: Claim 6 \n \t Location at original code in line: 64 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 7
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 7 \n \t Location at original code in line: 64 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
   item->next   = *list;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item->next), (void *)(intptr_t)(item->next), "item->next", 4, 0, 0, 0, "", 64); /** by FORTES **/

// FORTES: Claim 8
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 8 \n \t Location at original code in line: 65 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
   *list        = item;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(NULL), "list", 4, 0, 0, 0, "", 65); /** by FORTES **/
}


static void l2_insert(struct L2 **list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 5, 0, 0, 0, "", 68); /** by FORTES **/

   struct L2 *item = zalloc_or_die(sizeof(*item));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item), (void *)(intptr_t)(item), "item", 5, 0, 0, 0, "", 70); /** by FORTES **/

   do
   {
// FORTES: Claim 9
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)(NULL))), "[Failed]\n VIOLATED PROPERTY: Claim 9 \n \t Location at original code in line: 73 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      l3_insert(&item->down);
   } while (__VERIFIER_nondet_int());
// FORTES: Claim 10
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)(NULL))), "[Failed]\n VIOLATED PROPERTY: Claim 10 \n \t Location at original code in line: 76 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 11
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 11 \n \t Location at original code in line: 76 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
   item->next   = *list;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item->next), (void *)(intptr_t)(item->next), "item->next", 5, 0, 0, 0, "", 76); /** by FORTES **/

// FORTES: Claim 12
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 12 \n \t Location at original code in line: 77 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
   *list        = item;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(NULL), "list", 5, 0, 0, 0, "", 77); /** by FORTES **/
}


static void l1_insert(struct L1 **list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 6, 0, 0, 0, "", 80); /** by FORTES **/

   struct L1 *item = zalloc_or_die(sizeof(*item));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item), (void *)(intptr_t)(item), "item", 6, 0, 0, 0, "", 82); /** by FORTES **/

   do
   {
// FORTES: Claim 13
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)(NULL))), "[Failed]\n VIOLATED PROPERTY: Claim 13 \n \t Location at original code in line: 85 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      l2_insert(&item->down);
   } while (__VERIFIER_nondet_int());
// FORTES: Claim 14
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)(NULL))), "[Failed]\n VIOLATED PROPERTY: Claim 14 \n \t Location at original code in line: 88 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 15
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 15 \n \t Location at original code in line: 88 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
   item->next   = *list;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item->next), (void *)(intptr_t)(item->next), "item->next", 6, 0, 0, 0, "", 88); /** by FORTES **/

// FORTES: Claim 16
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 16 \n \t Location at original code in line: 89 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
   *list        = item;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(NULL), "list", 6, 0, 0, 0, "", 89); /** by FORTES **/
}


static void l0_insert(struct L0 **list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 7, 0, 0, 0, "", 92); /** by FORTES **/

   struct L0 *item = zalloc_or_die(sizeof(*item));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item), (void *)(intptr_t)(item), "item", 7, 0, 0, 0, "", 94); /** by FORTES **/

   do
   {
// FORTES: Claim 17
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)(NULL))), "[Failed]\n VIOLATED PROPERTY: Claim 17 \n \t Location at original code in line: 97 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      l1_insert(&item->down);
   } while (__VERIFIER_nondet_int());
// FORTES: Claim 18
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)(NULL))), "[Failed]\n VIOLATED PROPERTY: Claim 18 \n \t Location at original code in line: 100 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
   item->next   = *list;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item->next), (void *)(intptr_t)(item->next), "item->next", 7, 0, 0, 0, "", 100); /** by FORTES **/

   *list        = item;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(NULL), "list", 7, 0, 0, 0, "", 101); /** by FORTES **/
}


static void l4_destroy(struct L4 *list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 8, 0, 0, 0, "", 104); /** by FORTES **/

   do
   {
// FORTES: Claim 19
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list->down)), (void *)(intptr_t)((list->down)))), "[Failed]\n VIOLATED PROPERTY: Claim 19 \n \t Location at original code in line: 107 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 20
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 20 \n \t Location at original code in line: 107 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 21
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 21 \n \t Location at original code in line: 107 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(list->down), 107));                                                                /** by FORTES **/
      free(list->down);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list->down), (void *)(intptr_t)(list->down), "list->down", 8, 0, 1, 0, "", 107); /** by FORTES **/

// FORTES: Claim 22
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 22 \n \t Location at original code in line: 109 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      struct L4 *next = list->next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(next), (void *)(intptr_t)(next), "next", 8, 0, 0, 0, "", 109); /** by FORTES **/

// FORTES: Claim 23
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 23 \n \t Location at original code in line: 110 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(list), 110));                                                    /** by FORTES **/
      free(list);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 8, 0, 1, 0, "", 110); /** by FORTES **/

      list         = next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 8, 0, 0, 0, "", 111); /** by FORTES **/
   } while (list);
}


static void l3_destroy(struct L3 *list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 9, 0, 0, 0, "", 116); /** by FORTES **/

   do
   {
// FORTES: Claim 24
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 24 \n \t Location at original code in line: 119 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      l4_destroy(list->down);
// FORTES: Claim 25
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 25 \n \t Location at original code in line: 121 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      struct L3 *next = list->next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(next), (void *)(intptr_t)(next), "next", 9, 0, 0, 0, "", 121); /** by FORTES **/

// FORTES: Claim 26
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 26 \n \t Location at original code in line: 122 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(list), 122));                                                    /** by FORTES **/
      free(list);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 9, 0, 1, 0, "", 122); /** by FORTES **/

      list         = next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 9, 0, 0, 0, "", 123); /** by FORTES **/
   } while (list);
}


static void l2_destroy(struct L2 *list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 10, 0, 0, 0, "", 128); /** by FORTES **/

   do
   {
// FORTES: Claim 27
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 27 \n \t Location at original code in line: 131 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      l3_destroy(list->down);
// FORTES: Claim 28
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 28 \n \t Location at original code in line: 133 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      struct L2 *next = list->next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(next), (void *)(intptr_t)(next), "next", 10, 0, 0, 0, "", 133); /** by FORTES **/

// FORTES: Claim 29
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 29 \n \t Location at original code in line: 134 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(list), 134));                                                     /** by FORTES **/
      free(list);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 10, 0, 1, 0, "", 134); /** by FORTES **/

      list         = next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 10, 0, 0, 0, "", 135); /** by FORTES **/
   } while (list);
}


static void l1_destroy(struct L1 *list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 11, 0, 0, 0, "", 140); /** by FORTES **/

   do
   {
// FORTES: Claim 30
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 30 \n \t Location at original code in line: 143 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      l2_destroy(list->down);
// FORTES: Claim 31
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 31 \n \t Location at original code in line: 145 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      struct L1 *next = list->next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(next), (void *)(intptr_t)(next), "next", 11, 0, 0, 0, "", 145); /** by FORTES **/

// FORTES: Claim 32
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 32 \n \t Location at original code in line: 146 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(list), 146));                                                     /** by FORTES **/
      free(list);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 11, 0, 1, 0, "", 146); /** by FORTES **/

      list         = next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 11, 0, 0, 0, "", 147); /** by FORTES **/
   } while (list);
}


static void l0_destroy(struct L0 *list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 12, 0, 0, 0, "", 152); /** by FORTES **/

   do
   {
// FORTES: Claim 33
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 33 \n \t Location at original code in line: 155 \n \t Comments:   dereference failure: NULL pointer\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 34
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 34 \n \t Location at original code in line: 155 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      l1_destroy(list->down);
// FORTES: Claim 35
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 35 \n \t Location at original code in line: 157 \n \t Comments:   dereference failure: NULL pointer\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 36
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 36 \n \t Location at original code in line: 157 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      struct L0 *next = list->next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(next), (void *)(intptr_t)(next), "next", 12, 0, 0, 0, "", 157); /** by FORTES **/

// FORTES: Claim 37
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 37 \n \t Location at original code in line: 158 \n \t Comments:   dereference failure: NULL pointer\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 38
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 38 \n \t Location at original code in line: 158 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(list), 158));                                                     /** by FORTES **/
      free(list);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 12, 0, 1, 0, "", 158); /** by FORTES **/

      list         = next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 12, 0, 0, 0, "", 159); /** by FORTES **/
   } while (list);
}


int main()
{
   static struct L0 *list;

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(NULL), "list", 13, 0, 0, 0, "", 166); /** by FORTES **/

   do
   {
      l0_insert(&list);
   } while (__VERIFIER_nondet_int());
   l0_destroy(list);
   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 172)); /** by FORTES **/
}
