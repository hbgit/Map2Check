#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include <stdio.h>                      /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/

#include <stdlib.h>
#include <string.h>

extern void __VERIFIER_error(int linenumber);

extern int __VERIFIER_nondet_int(void);

typedef void *   list_t[2];

typedef list_t * list_p;

typedef enum
{
   LIST_BEG, LIST_END
} end_point_t;

typedef void *   item_t[2];

typedef item_t * item_p;

typedef enum
{
   ITEM_PREV, ITEM_NEXT
} direction_t;

typedef struct
{
   item_t head;
   char   text[0x100 + 1];
} *user_item_p;

int is_empty(list_p list)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 1, 0, 0, 0, "list_t", 27); /** by FORTES **/

// FORTES: Claim 1
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 29 \n \t Comments:   dereference failure: NULL pointer");
   int no_beg = !(*list)[LIST_BEG];
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(no_beg), (void *)(intptr_t)(NULL), "no_beg", 1, 0, 0, 0, "int", 29); /** by FORTES **/

// FORTES: Claim 2
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 2 \n \t Location at original code in line: 30 \n \t Comments:   dereference failure: NULL pointer");
   int no_end = !(*list)[LIST_END];
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(no_end), (void *)(intptr_t)(NULL), "no_end", 1, 0, 0, 0, "int", 30); /** by FORTES **/

   if (no_beg != no_end)
   {
// FORTES: Claim 3
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 3 \n \t Location at original code in line: 34 \n \t Comments:   dereference failure: free() of non-dynamic memory");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(list), 34));                                                          /** by FORTES **/
      free(list);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 1, 0, 1, 0, "list_t", 34); /** by FORTES **/
   }
   return(no_beg);
}


item_p create_item(end_point_t at, item_p *cursor)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(at), (void *)(intptr_t)(NULL), "at", 2, 0, 0, 0, "end_point_t", 39);      /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(cursor), (void *)(intptr_t)(cursor), "cursor", 2, 0, 0, 0, "item_t", 39); /** by FORTES **/

   user_item_p item = malloc(sizeof(*item));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item), (void *)(intptr_t)(item), "item", 2, 1, 0, 0, "user_item_p", 41); /** by FORTES **/

   if (!item)
   {
      assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 43)); /** by FORTES **/
      abort();
   }
   direction_t term_field;
   direction_t link_field;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(term_field), (void *)(intptr_t)(NULL), "term_field", 2, 0, 0, 0, "direction_t", 45); /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(link_field), (void *)(intptr_t)(NULL), "link_field", 2, 0, 0, 0, "direction_t", 45); /** by FORTES **/

   switch (at)
   {
   case LIST_BEG:

      link_field = ITEM_NEXT;
      term_field = ITEM_PREV;
      break;

   case LIST_END:

      link_field = ITEM_PREV;
      term_field = ITEM_NEXT;
      break;
   }
// FORTES: Claim 4
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 4 \n \t Location at original code in line: 60 \n \t Comments:   dereference failure: memory model not applicable (got `void * [2]', expected `void * [2] *')");
// FORTES: Claim 5
   
   //print_debug(list_LOG_mcf);
   
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((cursor)), (void *)(intptr_t)((cursor)))), "[Failed]\n VIOLATED PROPERTY: Claim 5 \n \t Location at original code in line: 60 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 6
   __MAP_FORTES_assert((intptr_t)link_field >= 0, "[Failed]\n VIOLATED PROPERTY: Claim 6 \n \t Location at original code in line: 60 \n \t Comments:   array lower bound");
// FORTES: Claim 7
   __MAP_FORTES_assert((intptr_t)link_field < 2, "[Failed]\n VIOLATED PROPERTY: Claim 7 \n \t Location at original code in line: 60 \n \t Comments:   array upper bound");
// FORTES: Claim 8
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 8 \n \t Location at original code in line: 60 \n \t Comments:   dereference failure: memory model not applicable (got `void * [2]', expected `void * [2] *')");
// FORTES: Claim 9
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((cursor)), (void *)(intptr_t)((cursor)))), "[Failed]\n VIOLATED PROPERTY: Claim 9 \n \t Location at original code in line: 60 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 10
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 10 \n \t Location at original code in line: 60 \n \t Comments:   dereference failure: invalid pointer");
   while (((*cursor) && (*(*cursor))[link_field]) && __VERIFIER_nondet_int())
   {
// FORTES: Claim 11
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 11 \n \t Location at original code in line: 61 \n \t Comments:   dereference failure: memory model not applicable (got `void * [2]', expected `void * [2] *')");
// FORTES: Claim 12
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((cursor)), (void *)(intptr_t)((cursor)))), "[Failed]\n VIOLATED PROPERTY: Claim 12 \n \t Location at original code in line: 61 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 13
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 13 \n \t Location at original code in line: 61 \n \t Comments:   dereference failure: invalid pointer");
      cursor       = (item_p *)(&(*(*cursor))[link_field]);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(cursor), (void *)(intptr_t)(cursor), "cursor", 2, 0, 0, 0, "item_t", 61); /** by FORTES **/
   }
// FORTES: Claim 14
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 14 \n \t Location at original code in line: 63 \n \t Comments:   dereference failure: memory model not applicable (got `void * [2]', expected `void * [2] *')");
// FORTES: Claim 15
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((cursor)), (void *)(intptr_t)((cursor)))), "[Failed]\n VIOLATED PROPERTY: Claim 15 \n \t Location at original code in line: 63 \n \t Comments:   dereference failure: invalid pointer");
   item_p link = *cursor;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(link), (void *)(intptr_t)(link), "link", 2, 0, 0, 0, "item_t", 63); /** by FORTES **/

// FORTES: Claim 16
   __MAP_FORTES_assert((intptr_t)link_field >= 0, "[Failed]\n VIOLATED PROPERTY: Claim 16 \n \t Location at original code in line: 64 \n \t Comments:   array lower bound");
// FORTES: Claim 17
   __MAP_FORTES_assert((intptr_t)link_field < 2, "[Failed]\n VIOLATED PROPERTY: Claim 17 \n \t Location at original code in line: 64 \n \t Comments:   array upper bound");
   item->head[link_field] = link;
   list_LOG_mcf           = mark_map_FORTES(list_LOG_mcf, (void *)&(item->head[link_field]), (void *)(intptr_t)(item->head[link_field]), "item->head[link_field]", 2, 0, 0, 0, "user_item_p", 64); /** by FORTES **/

// FORTES: Claim 18
   __MAP_FORTES_assert((intptr_t)term_field >= 0, "[Failed]\n VIOLATED PROPERTY: Claim 18 \n \t Location at original code in line: 65 \n \t Comments:   array lower bound");
// FORTES: Claim 19
   __MAP_FORTES_assert((intptr_t)term_field < 2, "[Failed]\n VIOLATED PROPERTY: Claim 19 \n \t Location at original code in line: 65 \n \t Comments:   array upper bound");
// FORTES: Claim 20
   __MAP_FORTES_assert((intptr_t)( int )term_field >= 0, "[Failed]\n VIOLATED PROPERTY: Claim 20 \n \t Location at original code in line: 65 \n \t Comments:   array lower bound");
// FORTES: Claim 21
   __MAP_FORTES_assert((intptr_t)( int )term_field < 2, "[Failed]\n VIOLATED PROPERTY: Claim 21 \n \t Location at original code in line: 65 \n \t Comments:   array upper bound");
// FORTES: Claim 22
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((link)), (void *)(intptr_t)((link)))) || (intptr_t)link == NULL, "[Failed]\n VIOLATED PROPERTY: Claim 22 \n \t Location at original code in line: 65 \n \t Comments:   dereference failure: invalid pointer");
   item->head[term_field] = link ? (*link)[term_field] : 0;
   list_LOG_mcf           = mark_map_FORTES(list_LOG_mcf, (void *)&(item->head[term_field]), (void *)(intptr_t)(item->head[term_field]), "item->head[term_field]", 2, 0, 0, 0, "user_item_p", 65); /** by FORTES **/

   item->text[0] = '\0';
   list_LOG_mcf  = mark_map_FORTES(list_LOG_mcf, (void *)&(item->text[0]), (void *)(intptr_t)(item->text[0]), "item->text[0]", 2, 0, 0, 0, "user_item_p", 66); /** by FORTES **/

   item_p head = &item->head;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(head), (void *)(intptr_t)(head), "head", 2, 0, 0, 0, "item_t", 68); /** by FORTES **/

   if (link)
   {
// FORTES: Claim 23
      __MAP_FORTES_assert((intptr_t)term_field >= 0, "[Failed]\n VIOLATED PROPERTY: Claim 23 \n \t Location at original code in line: 71 \n \t Comments:   array lower bound");
// FORTES: Claim 24
      __MAP_FORTES_assert((intptr_t)term_field < 2, "[Failed]\n VIOLATED PROPERTY: Claim 24 \n \t Location at original code in line: 71 \n \t Comments:   array upper bound");
// FORTES: Claim 25
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((link)), (void *)(intptr_t)((link)))), "[Failed]\n VIOLATED PROPERTY: Claim 25 \n \t Location at original code in line: 71 \n \t Comments:   dereference failure: invalid pointer");
      (*link)[term_field] = head;
      list_LOG_mcf        = mark_map_FORTES(list_LOG_mcf, (void *)&((*link)[term_field]), (void *)(intptr_t)((*link)[term_field]), "(*link)[term_field]", 2, 0, 0, 0, "item_t", 71); /** by FORTES **/
   }
// FORTES: Claim 26
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 26 \n \t Location at original code in line: 73 \n \t Comments:   dereference failure: memory model not applicable (got `void * [2]', expected `void * [2] *')");
// FORTES: Claim 27
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((cursor)), (void *)(intptr_t)((cursor)))), "[Failed]\n VIOLATED PROPERTY: Claim 27 \n \t Location at original code in line: 73 \n \t Comments:   dereference failure: invalid pointer");
   *cursor      = head;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(cursor), (void *)(intptr_t)(cursor), "cursor", 2, 0, 0, 0, "item_t", 73); /** by FORTES **/

   return(head);
}


void append_one(list_p list, end_point_t to)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 3, 0, 0, 0, "list_t", 77);  /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(to), (void *)(intptr_t)(NULL), "to", 3, 0, 0, 0, "end_point_t", 77); /** by FORTES **/

// FORTES: Claim 28
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 28 \n \t Location at original code in line: 79 \n \t Comments:   dereference failure: NULL pointer");
   item_p *cursor = (item_p *)(&(*list)[to]);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(cursor), (void *)(intptr_t)(cursor), "cursor", 3, 0, 0, 0, "item_t", 79); /** by FORTES **/

   item_p item = create_item(to, cursor);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item), (void *)(intptr_t)(item), "item", 3, 0, 0, 0, "item_t", 80); /** by FORTES **/

   if (0 == (*item)[ITEM_PREV])
   {
// FORTES: Claim 29
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 29 \n \t Location at original code in line: 83 \n \t Comments:   dereference failure: NULL pointer");
      (*list)[LIST_BEG] = item;
      list_LOG_mcf      = mark_map_FORTES(list_LOG_mcf, (void *)&((*list)[LIST_BEG]), (void *)(intptr_t)((*list)[LIST_BEG]), "(*list)[LIST_BEG]", 3, 0, 0, 0, "list_t", 83); /** by FORTES **/
   }
// FORTES: Claim 30
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 30 \n \t Location at original code in line: 85 \n \t Comments:   dereference failure: NULL pointer");
   if (0 == (*list)[ITEM_NEXT])
   {
// FORTES: Claim 31
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 31 \n \t Location at original code in line: 86 \n \t Comments:   dereference failure: NULL pointer");
      (*list)[LIST_END] = item;
      list_LOG_mcf      = mark_map_FORTES(list_LOG_mcf, (void *)&((*list)[LIST_END]), (void *)(intptr_t)((*list)[LIST_END]), "(*list)[LIST_END]", 3, 0, 0, 0, "list_t", 86); /** by FORTES **/
   }
}


void remove_one(list_p list, end_point_t from)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(list), (void *)(intptr_t)(list), "list", 4, 0, 0, 0, "list_t", 89);      /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(from), (void *)(intptr_t)(NULL), "from", 4, 0, 0, 0, "end_point_t", 89); /** by FORTES **/

   if (is_empty(list))
   {
      return;
   }
// FORTES: Claim 32
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 32 \n \t Location at original code in line: 95 \n \t Comments:   dereference failure: NULL pointer");
// FORTES: Claim 33
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 33 \n \t Location at original code in line: 95 \n \t Comments:   dereference failure: NULL pointer");
   if ((*list)[LIST_BEG] == (*list)[LIST_END])
   {
// FORTES: Claim 34
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 34 \n \t Location at original code in line: 96 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 35
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 35 \n \t Location at original code in line: 96 \n \t Comments:   dereference failure: NULL pointer");
// FORTES: Claim 36
      __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 36 \n \t Location at original code in line: 96 \n \t Comments:   dereference failure: NULL pointer");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)((*list)[LIST_BEG]), 96));                                                                                    /** by FORTES **/
      free((*list)[LIST_BEG]);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&((*list)[LIST_BEG]), (void *)(intptr_t)((*list)[LIST_BEG]), "(*list)[LIST_BEG]", 4, 0, 1, 0, "list_t", 96); /** by FORTES **/

      memset(*list, 0, sizeof(*list));
      return;
   }
   const direction_t next_field = LIST_BEG == from ? ITEM_NEXT : ITEM_PREV;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(next_field), (void *)(intptr_t)(NULL), "next_field", 4, 0, 0, 0, "direction_t", 101); /** by FORTES **/

   const direction_t term_field = LIST_END == from ? ITEM_NEXT : ITEM_PREV;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(term_field), (void *)(intptr_t)(NULL), "term_field", 4, 0, 0, 0, "direction_t", 102); /** by FORTES **/

// FORTES: Claim 38
   __MAP_FORTES_assert((intptr_t)from >= 0, "[Failed]\n VIOLATED PROPERTY: Claim 38 \n \t Location at original code in line: 104 \n \t Comments:   array lower bound");
// FORTES: Claim 39
   __MAP_FORTES_assert((intptr_t)from < 2, "[Failed]\n VIOLATED PROPERTY: Claim 39 \n \t Location at original code in line: 104 \n \t Comments:   array upper bound");
// FORTES: Claim 40
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 40 \n \t Location at original code in line: 104 \n \t Comments:   dereference failure: NULL pointer");
   item_p item = (*list)[from];
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item), (void *)(intptr_t)(item), "item", 4, 0, 0, 0, "item_t", 104); /** by FORTES **/

// FORTES: Claim 41
   __MAP_FORTES_assert((intptr_t)next_field >= 0, "[Failed]\n VIOLATED PROPERTY: Claim 41 \n \t Location at original code in line: 105 \n \t Comments:   array lower bound");
// FORTES: Claim 42
   __MAP_FORTES_assert((intptr_t)next_field < 2, "[Failed]\n VIOLATED PROPERTY: Claim 42 \n \t Location at original code in line: 105 \n \t Comments:   array upper bound");
// FORTES: Claim 43
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)((item)))), "[Failed]\n VIOLATED PROPERTY: Claim 43 \n \t Location at original code in line: 105 \n \t Comments:   dereference failure: invalid pointer");
   item_p next = (*item)[next_field];
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(next), (void *)(intptr_t)(next), "next", 4, 0, 0, 0, "item_t", 105); /** by FORTES **/

// FORTES: Claim 44
   __MAP_FORTES_assert((intptr_t)term_field >= 0, "[Failed]\n VIOLATED PROPERTY: Claim 44 \n \t Location at original code in line: 106 \n \t Comments:   array lower bound");
// FORTES: Claim 45
   __MAP_FORTES_assert((intptr_t)term_field < 2, "[Failed]\n VIOLATED PROPERTY: Claim 45 \n \t Location at original code in line: 106 \n \t Comments:   array upper bound");
// FORTES: Claim 46
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((next)), (void *)(intptr_t)((next)))), "[Failed]\n VIOLATED PROPERTY: Claim 46 \n \t Location at original code in line: 106 \n \t Comments:   dereference failure: invalid pointer");
   (*next)[term_field] = 0;
   list_LOG_mcf        = mark_map_FORTES(list_LOG_mcf, (void *)&((*next)[term_field]), (void *)(intptr_t)((*next)[term_field]), "(*next)[term_field]", 4, 0, 0, 0, "item_t", 106); /** by FORTES **/

// FORTES: Claim 47
   __MAP_FORTES_assert((intptr_t)from >= 0, "[Failed]\n VIOLATED PROPERTY: Claim 47 \n \t Location at original code in line: 107 \n \t Comments:   array lower bound");
// FORTES: Claim 48
   __MAP_FORTES_assert((intptr_t)from < 2, "[Failed]\n VIOLATED PROPERTY: Claim 48 \n \t Location at original code in line: 107 \n \t Comments:   array upper bound");
// FORTES: Claim 49
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 49 \n \t Location at original code in line: 107 \n \t Comments:   dereference failure: NULL pointer");
   (*list)[from] = next;
   list_LOG_mcf  = mark_map_FORTES(list_LOG_mcf, (void *)&((*list)[from]), (void *)(intptr_t)((*list)[from]), "(*list)[from]", 4, 0, 0, 0, "list_t", 107); /** by FORTES **/

// FORTES: Claim 50
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((item)), (void *)(intptr_t)((item)))), "[Failed]\n VIOLATED PROPERTY: Claim 50 \n \t Location at original code in line: 109 \n \t Comments:   dereference failure: invalid pointer");
   assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(item), 109));                                                          /** by FORTES **/
   free(item);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(item), (void *)(intptr_t)(item), "item", 4, 0, 1, 0, "item_t", 109); /** by FORTES **/
}


end_point_t rand_end_point(void)
{
   if (__VERIFIER_nondet_int())
   {
      return(LIST_BEG);
   }
   else
   {
      return(LIST_END);
   }
}


int main()
{
   static list_t list;

   while (__VERIFIER_nondet_int())
   {
      while (__VERIFIER_nondet_int())
      {
// FORTES: Claim 37
         __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 37 \n \t Location at original code in line: 126 \n \t Comments:   dereference failure: NULL pointer");
         append_one(&list, rand_end_point());
      }
      while (__VERIFIER_nondet_int())
      {
         remove_one(&list, rand_end_point());
      }
   }
   end_point_t end_point;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(end_point), (void *)(intptr_t)(NULL), "end_point", 5, 0, 0, 0, "end_point_t", 132); /** by FORTES **/

   direction_t direction;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(direction), (void *)(intptr_t)(NULL), "direction", 5, 0, 0, 0, "direction_t", 133); /** by FORTES **/

   if (__VERIFIER_nondet_int())
   {
      end_point = LIST_BEG;
      direction = ITEM_NEXT;
   }
   else
   {
      end_point = LIST_END;
      direction = ITEM_PREV;
   }
// FORTES: Claim 51
   __MAP_FORTES_assert((intptr_t)end_point >= 0, "[Failed]\n VIOLATED PROPERTY: Claim 51 \n \t Location at original code in line: 147 \n \t Comments:   array `list' lower bound");
// FORTES: Claim 52
   __MAP_FORTES_assert((intptr_t)end_point < 2, "[Failed]\n VIOLATED PROPERTY: Claim 52 \n \t Location at original code in line: 147 \n \t Comments:   array `list' upper bound");
   item_p cursor = list[end_point];
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(cursor), (void *)(intptr_t)(cursor), "cursor", 5, 0, 0, 0, "item_t", 147); /** by FORTES **/

   while (cursor)
   {
// FORTES: Claim 53
      __MAP_FORTES_assert((intptr_t)direction >= 0, "[Failed]\n VIOLATED PROPERTY: Claim 53 \n \t Location at original code in line: 149 \n \t Comments:   array lower bound");
// FORTES: Claim 54
      __MAP_FORTES_assert((intptr_t)direction < 2, "[Failed]\n VIOLATED PROPERTY: Claim 54 \n \t Location at original code in line: 149 \n \t Comments:   array upper bound");
// FORTES: Claim 55
      __MAP_FORTES_assert(!(cursor == NULL), "[Failed]\n VIOLATED PROPERTY: Claim 55 \n \t Location at original code in line: 149 \n \t Comments:   dereference failure: NULL pointer");
// FORTES: Claim 56
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((cursor)), (void *)(intptr_t)((cursor)))), "[Failed]\n VIOLATED PROPERTY: Claim 56 \n \t Location at original code in line: 149 \n \t Comments:   dereference failure: invalid pointer");
      item_p next = (*cursor)[direction];
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(next), (void *)(intptr_t)(next), "next", 5, 0, 0, 0, "item_t", 149); /** by FORTES **/

// FORTES: Claim 57
      __MAP_FORTES_assert(!(cursor == NULL), "[Failed]\n VIOLATED PROPERTY: Claim 57 \n \t Location at original code in line: 150 \n \t Comments:   dereference failure: NULL pointer");
// FORTES: Claim 58
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((cursor)), (void *)(intptr_t)((cursor)))), "[Failed]\n VIOLATED PROPERTY: Claim 58 \n \t Location at original code in line: 150 \n \t Comments:   dereference failure: invalid pointer");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(cursor), 150));                                                              /** by FORTES **/
      free(cursor);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(cursor), (void *)(intptr_t)(cursor), "cursor", 5, 0, 1, 0, "item_t", 150); /** by FORTES **/

      cursor       = next;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(cursor), (void *)(intptr_t)(cursor), "cursor", 5, 0, 0, 0, "item_t", 151); /** by FORTES **/
   }
   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 154));                                                                                     /** by FORTES **/
   return(0);

   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 154)); /** by FORTES **/
}
