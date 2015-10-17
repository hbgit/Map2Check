#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include <stdio.h>                      /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/

#include <stdlib.h>

extern void __VERIFIER_error(int linenumber);

extern int __VERIFIER_nondet_int(void);

struct list_head
{
   struct list_head *next;
   struct list_head *prev;
};

inline static int list_empty(struct list_head *head)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(head), (void *)(intptr_t)(head), "head", 1, 0, 0, 0, "", 28); /** by FORTES **/

   return(head->next == head);
}


inline static void __list_add(struct list_head *new, struct list_head *prev, struct list_head *next)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(new), (void *)(intptr_t)(new), "new", 2, 0, 0, 0, "", 33); /** by FORTES **/

// FORTES: Claim 19
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((next)), (void *)(intptr_t)((next)))), "[Failed]\n VIOLATED PROPERTY: Claim 19 \n \t Location at original code in line: 103 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 22
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((next)), (void *)(intptr_t)((next)))), "[Failed]\n VIOLATED PROPERTY: Claim 22 \n \t Location at original code in line: 114 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
   next->prev   = new;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(next->prev), (void *)(intptr_t)(next->prev), "next->prev", 2, 0, 0, 0, "", 37); /** by FORTES **/

// FORTES: Claim 20
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((new)), (void *)(intptr_t)((new)))), "[Failed]\n VIOLATED PROPERTY: Claim 20 \n \t Location at original code in line: 114 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
   new->next    = next;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(new->next), (void *)(intptr_t)(new->next), "new->next", 2, 0, 0, 0, "", 38); /** by FORTES **/

// FORTES: Claim 21
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((new)), (void *)(intptr_t)((new)))), "[Failed]\n VIOLATED PROPERTY: Claim 21 \n \t Location at original code in line: 114 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
   new->prev    = prev;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(new->prev), (void *)(intptr_t)(new->prev), "new->prev", 2, 0, 0, 0, "", 39); /** by FORTES **/

   prev->next   = new;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(prev->next), (void *)(intptr_t)(prev->next), "prev->next", 2, 0, 0, 0, "", 40); /** by FORTES **/
}


inline static void __list_del(struct list_head *prev, struct list_head *next)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(prev), (void *)(intptr_t)(prev), "prev", 3, 0, 0, 0, "", 43); /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(next), (void *)(intptr_t)(next), "next", 3, 0, 0, 0, "", 43); /** by FORTES **/

// FORTES: Claim 17
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((next)), (void *)(intptr_t)((next)))), "[Failed]\n VIOLATED PROPERTY: Claim 17 \n \t Location at original code in line: 103 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
   next->prev   = prev;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(next->prev), (void *)(intptr_t)(next->prev), "next->prev", 3, 0, 0, 0, "", 45); /** by FORTES **/

// FORTES: Claim 18
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((prev)), (void *)(intptr_t)((prev)))), "[Failed]\n VIOLATED PROPERTY: Claim 18 \n \t Location at original code in line: 103 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 23
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((prev)), (void *)(intptr_t)((prev)))), "[Failed]\n VIOLATED PROPERTY: Claim 23 \n \t Location at original code in line: 114 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
   prev->next   = next;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(prev->next), (void *)(intptr_t)(prev->next), "prev->next", 3, 0, 0, 0, "", 46); /** by FORTES **/
}


inline static void list_add(struct list_head *new, struct list_head *head)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(new), (void *)(intptr_t)(new), "new", 4, 0, 0, 0, "", 49);    /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(head), (void *)(intptr_t)(head), "head", 4, 0, 0, 0, "", 49); /** by FORTES **/

   __list_add(new, head, head->next);
}


inline static void list_del(struct list_head *entry)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(entry), (void *)(intptr_t)(entry), "entry", 5, 0, 0, 0, "", 54); /** by FORTES **/

   __list_del(entry->prev, entry->next);
   entry->next  = (void *)0;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(entry->next), (void *)(intptr_t)(entry->next), "entry->next", 5, 0, 0, 0, "", 57); /** by FORTES **/

   entry->prev  = (void *)0;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(entry->prev), (void *)(intptr_t)(entry->prev), "entry->prev", 5, 0, 0, 0, "", 58); /** by FORTES **/
}


inline static void list_move(struct list_head *list, struct list_head *head)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 6, 0, 0, 0, "", 61); /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(head), (void *)(intptr_t)(head), "head", 6, 0, 0, 0, "", 61); /** by FORTES **/

// FORTES: Claim 15
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 15 \n \t Location at original code in line: 103 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 16
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((list)), (void *)(intptr_t)((list)))), "[Failed]\n VIOLATED PROPERTY: Claim 16 \n \t Location at original code in line: 103 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
   __list_del(list->prev, list->next);
   list_add(list, head);
}


struct node
{
   int              value;
   struct list_head linkage;
};

struct list_head gl_list = { &gl_list, &gl_list };

static void gl_insert(int value)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(value), (void *)(intptr_t)(NULL), "value", 8, 0, 0, 0, "int", 74); /** by FORTES **/

   struct node *node = malloc(sizeof(*node));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(node), (void *)(intptr_t)(node), "node", 8, 1, 0, 0, "", 76); /** by FORTES **/

   if (!node)
   {
      assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 78)); /** by FORTES **/
      abort();
   }
   node->value  = value;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(node->value), (void *)(intptr_t)(node->value), "node->value", 8, 0, 0, 0, "", 80); /** by FORTES **/

   list_add(&node->linkage, &gl_list);
}


static void gl_read()
{
   do
   {
      gl_insert(__VERIFIER_nondet_int());
   } while (__VERIFIER_nondet_int());
}


static void gl_destroy()
{
   struct list_head *next;

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(next), (void *)(intptr_t)(NULL), "next", 9, 0, 0, 0, "", 94); /** by FORTES **/

   while ((&gl_list) != (next = gl_list.next))
   {
// FORTES: Claim 1
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((next)), (void *)(intptr_t)(NULL))), "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 96 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
      gl_list.next = next->next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(gl_list.next), (void *)(intptr_t)(gl_list.next), "gl_list.next", 9, 0, 0, 0, "", 96); /** by FORTES **/

// FORTES: Claim 2
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 2 \n \t Location at original code in line: 96 \n \t Comments:   dereference failure: free() of non-dynamic memory\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 3
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 3 \n \t Location at original code in line: 97 \n \t Comments:   dereference failure: offset not zero (non-array-object)\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 4
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 4 \n \t Location at original code in line: 97 \n \t Comments:   dereference failure: invalid pointer\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 5
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 5 \n \t Location at original code in line: 97 \n \t Comments:   dereference failure: dynamic object lower bound\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 6
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 6 \n \t Location at original code in line: 97 \n \t Comments:   dereference failure: dynamic object upper bound\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 7
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 7 \n \t Location at original code in line: 97 \n \t Comments:   dereference failure: free() of non-dynamic memory\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 8
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 8 \n \t Location at original code in line: 97 \n \t Comments:   dereference failure: offset not zero (non-array-object)\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 9
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 9 \n \t Location at original code in line: 97 \n \t Comments:   dereference failure: NULL pointer\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 10
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 10 \n \t Location at original code in line: 97 \n \t Comments:   dereference failure: NULL pointer\n \n FALSE(valid-deref) \n ");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)((struct node *)(((char *)next) - ((unsigned long)(&((struct node *)0)->linkage)))), 97));                                                                /** by FORTES **/
      free((struct node *)(((char *)next) - ((unsigned long)(&((struct node *)0)->linkage))));
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(gl_destroy), (void *)(intptr_t)((struct node *)(((char *)next) - ((unsigned long)(&((struct node *)0)->linkage)))), "gl_destroy", 9, 0, 1, 0, "", 97); /** by FORTES **/
   }
}


static int val_from_node(struct list_head *head)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(head), (void *)(intptr_t)(head), "head", 10, 0, 0, 0, "", 101); /** by FORTES **/

// FORTES: Claim 11
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 11 \n \t Location at original code in line: 97 \n \t Comments:   dereference failure: NULL pointer\n \n FALSE(valid-deref) \n ");
   struct node *entry = (struct node *)(((char *)head) - ((unsigned long)(&((struct node *)0)->linkage)));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(entry), (void *)(intptr_t)(entry), "entry", 10, 0, 0, 0, "", 102); /** by FORTES **/

// FORTES: Claim 12
   __MAP_FORTES_assert(!(entry == &gl_list), "[Failed]\n VIOLATED PROPERTY: Claim 12 \n \t Location at original code in line: 97 \n \t Comments:   dereference failure: memory model not applicable (got `struct list_head', expected `struct node')\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 13
   __MAP_FORTES_assert(!((intptr_t)(entry) < 0) || !((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&entry, (void *)(intptr_t)entry))), "[Failed]\n VIOLATED PROPERTY: Claim 13 \n \t Location at original code in line: 102 \n \t Comments:   dereference failure: dynamic object lower bound\n \n FALSE(valid-deref) \n ");
// FORTES: Claim 14
   __MAP_FORTES_assert(!((intptr_t)(entry) >= ((intptr_t)(entry))) || !((IS_DYNAMIC_OBJECT_FORTES(list_LOG_mcf, (void *)&entry, (void *)(intptr_t)entry))), "[Failed]\n VIOLATED PROPERTY: Claim 14 \n \t Location at original code in line: 103 \n \t Comments:   dereference failure: dynamic object upper bound\n \n FALSE(valid-deref) \n ");
   return(entry->value);
}


static struct list_head *gl_seek_max()
{
   if (list_empty(&gl_list))
   {
      return(0);
   }
   struct list_head *pos;
   struct list_head *max_pos = gl_list.next;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(pos), (void *)(intptr_t)(NULL), "pos", 11, 0, 0, 0, "", 111);            /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(max_pos), (void *)(intptr_t)(max_pos), "max_pos", 11, 0, 0, 0, "", 111); /** by FORTES **/

   int max = val_from_node(max_pos);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(max), (void *)(intptr_t)(NULL), "max", 11, 0, 0, 0, "int", 112); /** by FORTES **/

   for (pos = max_pos->next; (&gl_list) != pos; pos = pos->next)
   {
      const int value = val_from_node(pos);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(value), (void *)(intptr_t)(NULL), "value", 11, 0, 0, 0, "int", 115); /** by FORTES **/

      if (value < max)
      {
         continue;
      }
      max_pos      = pos;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(max_pos), (void *)(intptr_t)(max_pos), "max_pos", 11, 0, 0, 0, "", 119); /** by FORTES **/

      max = value;
   }
   return(max_pos);
}


static void gl_sort()
{
   if (list_empty(&gl_list))
   {
      return;
   }
   struct list_head dst = { &dst, &dst };
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(dst), (void *)(intptr_t)(NULL), "dst", 12, 0, 0, 0, "", 131); /** by FORTES **/

   struct list_head *max_pos;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(max_pos), (void *)(intptr_t)(NULL), "max_pos", 12, 0, 0, 0, "", 133); /** by FORTES **/

   while (max_pos = gl_seek_max())
   {
      list_move(max_pos, &dst);
   }
   list_add(&gl_list, &dst);
   list_del(&dst);
}


int main()
{
   gl_read();
   gl_sort();
   gl_destroy();
   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 148)); /** by FORTES **/
   return(0);

   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 148)); /** by FORTES **/
}
