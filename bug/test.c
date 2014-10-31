#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include <stdio.h>                      /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/

#include <stdlib.h>

extern void __VERIFIER_error(int linenumber);

typedef struct
{
   void *lo;
   void *hi;
} TData;

static void alloc_data(TData *pdata)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(pdata), (void *)(intptr_t)(pdata), "pdata", 1, 0, 0, 0, "TData", 10); /** by FORTES **/

   pdata->lo    = malloc(16);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(pdata->lo), (void *)(intptr_t)(pdata->lo), "pdata->lo", 1, 1, 0, 0, "TData", 12); /** by FORTES **/

   pdata->hi    = malloc(24);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(pdata->hi), (void *)(intptr_t)(pdata->hi), "pdata->hi", 1, 1, 0, 0, "TData", 13); /** by FORTES **/
}


static void free_data(TData *data)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(data), (void *)(intptr_t)(data), "data", 2, 0, 0, 0, "TData", 16); /** by FORTES **/

   void *lo = data->lo;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(lo), (void *)(intptr_t)(lo), "lo", 2, 0, 0, 0, "void", 18); /** by FORTES **/

   void *hi = data->hi;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(hi), (void *)(intptr_t)(hi), "hi", 2, 0, 0, 0, "void", 19); /** by FORTES **/

   if (lo == hi)
   {
// FORTES: Claim 1
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((lo)), (void *)(intptr_t)((lo)))), "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 22 \n \t Comments:   dereference failure: invalid pointer");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(lo), 22));                                                    /** by FORTES **/
      free(lo);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(lo), (void *)(intptr_t)(lo), "lo", 2, 0, 1, 0, "void", 22); /** by FORTES **/

// FORTES: Claim 2
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&((hi)), (void *)(intptr_t)((hi)))), "[Failed]\n VIOLATED PROPERTY: Claim 2 \n \t Location at original code in line: 23 \n \t Comments:   dereference failure: invalid pointer");
      assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(hi), 23));                                                    /** by FORTES **/
      free(hi);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(hi), (void *)(intptr_t)(hi), "hi", 2, 0, 1, 0, "void", 23); /** by FORTES **/
   }
   data->lo     = 0;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(data->lo), (void *)(intptr_t)(data->lo), "data->lo", 2, 0, 0, 0, "TData", 26); /** by FORTES **/

   data->hi     = 0;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(data->hi), (void *)(intptr_t)(data->hi), "data->hi", 2, 0, 0, 0, "TData", 27); /** by FORTES **/
}


int main()
{
   TData data;

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(data), (void *)(intptr_t)(NULL), "data", 3, 0, 0, 0, "TData", 31); /** by FORTES **/

   alloc_data(&data);
   free_data(&data);
   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 34)); /** by FORTES **/
   return(0);

   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 34)); /** by FORTES **/
}
