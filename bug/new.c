#include <stdint.h>                     /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main()
{
   int a[] = { 1, 2, 3 };

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(a), (void *)(intptr_t)(NULL), "a", 1, 0, 0, 0, "int", 6); /** by FORTES **/

// FORTES: Claim 1
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 7 \n \t Comments:   assertion 0");
   assert(0);
}
