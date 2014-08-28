#include <assert.h> /** by FORTES **/
#include <stdint.h> /** by FORTES **/
#include <stdio.h> /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/ 

#include <stdlib.h>

typedef struct 
{
  int *lo;
  int *hi;
} TData;

static void alloc_data(TData *pdata)
{
list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(pdata),(void *)(intptr_t)(pdata),1,0,0,0,8); /** by FORTES **/ 

pdata->lo = (int *) malloc(sizeof(int));
list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(pdata->lo),(void *)(intptr_t)(pdata->lo),1,1,0,0,10); /** by FORTES **/ 

pdata->hi = (int *) malloc(sizeof(int));
list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(pdata->hi),(void *)(intptr_t)(pdata->hi),1,1,0,0,11); /** by FORTES **/ 

// FORTES: Claim 1
__MAP_FORTES_assert((1) , "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 13 \n \t Comments:   dereference failure: invalid pointer");
*pdata->lo = 4;
list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(*pdata->lo),(void *)(intptr_t)(*pdata->lo),1,0,0,0,13); /** by FORTES **/ 

// FORTES: Claim 2
__MAP_FORTES_assert((1) , "[Failed]\n VIOLATED PROPERTY: Claim 2 \n \t Location at original code in line: 14 \n \t Comments:   dereference failure: invalid pointer");
*pdata->hi = 8;
list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(*pdata->hi),(void *)(intptr_t)(*pdata->hi),1,0,0,0,14); /** by FORTES **/ 

}

static void free_data(TData *data)
{
list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(data),(void *)(intptr_t)(data),2,0,0,0,17); /** by FORTES **/ 

int *lo = data->lo;
list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(lo),(void *)(intptr_t)(lo),2,0,0,0,19); /** by FORTES **/ 

int *hi = data->hi;
list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(hi),(void *)(intptr_t)(hi),2,0,0,0,20); /** by FORTES **/ 

// FORTES: Claim 3
__MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES( list_LOG_mcf, (void *)&(lo), (void *)(intptr_t)(lo) )) , "[Failed]\n VIOLATED PROPERTY: Claim 3 \n \t Location at original code in line: 22 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 4
__MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES( list_LOG_mcf, (void *)&(hi), (void *)(intptr_t)(hi) )) , "[Failed]\n VIOLATED PROPERTY: Claim 4 \n \t Location at original code in line: 22 \n \t Comments:   dereference failure: invalid pointer");
if ((*lo) >= (*hi)){
// FORTES: Claim 5
__MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES( list_LOG_mcf, (void *)&(  lo), (void *)(intptr_t)(  lo) )) , "[Failed]\n VIOLATED PROPERTY: Claim 5 \n \t Location at original code in line: 23 \n \t Comments:   dereference failure: invalid pointer");
assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(lo),23)); /** by FORTES **/
free(lo);
list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(lo),(void *)(intptr_t)(lo),2,0,1,0,23); /** by FORTES **/ 

// FORTES: Claim 6
__MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES( list_LOG_mcf, (void *)&(  hi), (void *)(intptr_t)(  hi) )) , "[Failed]\n VIOLATED PROPERTY: Claim 6 \n \t Location at original code in line: 24 \n \t Comments:   dereference failure: invalid pointer");
assert(INVALID_FREE(list_LOG_mcf, (void *)(intptr_t)(hi),24)); /** by FORTES **/
free(hi);
list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(hi),(void *)(intptr_t)(hi),2,0,1,0,24); /** by FORTES **/ 

}
data->lo = 0;
list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(data->lo),(void *)(intptr_t)(data->lo),2,0,0,0,27); /** by FORTES **/ 

data->hi = 0;
list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(data->hi),(void *)(intptr_t)(data->hi),2,0,0,0,28); /** by FORTES **/ 

}

int main()
{
TData data;
list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(data),(void *)(intptr_t)(data),3,0,0,0,32); /** by FORTES **/ 

alloc_data(&data);
free_data(&data);
assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 41 )); /** by FORTES **/
return 0;
assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 35 )); /** by FORTES **/
}

