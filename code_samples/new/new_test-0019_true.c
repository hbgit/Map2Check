#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include <stdio.h>                      /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/

#include <stdlib.h>

typedef struct
{
   void *lo;
   void *hi;
} TData;

static void alloc_data(TData *pdata)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(pdata), (void *)(intptr_t)(pdata), 1, 0, 0, 0, 8); /** by FORTES **/

   pdata->lo    = malloc(16);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(pdata->lo), (void *)(intptr_t)(pdata->lo), 1, 1, 0, 0, 10); /** by FORTES **/

   pdata->hi    = malloc(24);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(pdata->hi), (void *)(intptr_t)(pdata->hi), 1, 1, 0, 0, 11); /** by FORTES **/
}


static void free_data(TData data)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(data), (void *)(intptr_t)(data), 2, 0, 0, 0, 14); /** by FORTES **/

   void *lo = data.lo;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(lo), (void *)(intptr_t)(lo), 2, 0, 0, 0, 16); /** by FORTES **/

   void *hi = data.hi;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(hi), (void *)(intptr_t)(hi), 2, 0, 0, 0, 17); /** by FORTES **/

   if (lo == hi)
   {
      return;
   }
   assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(lo), 22));                                      /** by FORTES **/
   free(lo);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(lo), (void *)(intptr_t)(lo), 2, 0, 1, 0, 22); /** by FORTES **/

   assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(hi), 23));                                      /** by FORTES **/
   free(hi);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(hi), (void *)(intptr_t)(hi), 2, 0, 1, 0, 23); /** by FORTES **/
}


int main()
{
   TData data;

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(data), (void *)(intptr_t)(data), 3, 0, 0, 0, 27); /** by FORTES **/

   alloc_data(&data);
   free_data(data);
   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 37)); /** by FORTES **/
   return(0);

   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 31)); /** by FORTES **/
}
