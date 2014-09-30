#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>

typedef unsigned char   BYTE;

int a[5];

int b[6];

void return_foo(int c)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(c), (void *)(intptr_t)(NULL), 1, 0, 0, 0, "int", 11); /** by FORTES **/

   BYTE b1;
   BYTE b2;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(b1), (void *)(intptr_t)(NULL), 1, 0, 0, 0, "unsigned char", 12); /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(b2), (void *)(intptr_t)(NULL), 1, 0, 0, 0, "unsigned char", 12); /** by FORTES **/

   int i;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(i), (void *)(intptr_t)(NULL), 1, 0, 0, 0, "int", 13); /** by FORTES **/

   for (i = 0; i < 5; i++)
   {
// FORTES: Claim 1
      __MAP_FORTES_assert((intptr_t)i >= 0, "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 15 \n \t Comments:   array `a' lower bound");
// FORTES: Claim 2
      __MAP_FORTES_assert((intptr_t)i < 5, "[Failed]\n VIOLATED PROPERTY: Claim 2 \n \t Location at original code in line: 15 \n \t Comments:   array `a' upper bound");
      a[i]         = i;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(a), (void *)(intptr_t)(NULL), 1, 0, 0, 0, "int", 15); /** by FORTES **/
   }
   b1           = '&';
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(b1), (void *)(intptr_t)(NULL), 1, 0, 0, 0, "unsigned char", 17); /** by FORTES **/
}


int main()
{
   unsigned int ka;

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(ka), (void *)(intptr_t)(NULL), 2, 0, 0, 0, "unsigned int", 23); /** by FORTES **/

   int i;
   int j;
   int temp;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(i), (void *)(intptr_t)(NULL), 2, 0, 0, 0, "int", 24);    /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(j), (void *)(intptr_t)(NULL), 2, 0, 0, 0, "int", 24);    /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(temp), (void *)(intptr_t)(NULL), 2, 0, 0, 0, "int", 24); /** by FORTES **/

   short unsigned usa = 65535;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(usa), (void *)(intptr_t)(NULL), 2, 0, 0, 0, "unsigned short", 25); /** by FORTES **/

   usa          = 11111;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(usa), (void *)(intptr_t)(NULL), 2, 0, 0, 0, "unsigned short", 26); /** by FORTES **/

   return_foo(3);
   int *p = malloc(10);
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(p), (void *)(intptr_t)(p), 2, 1, 0, 0, "int", 29); /** by FORTES **/

   for (i = 0; i < 5; i++)
   {
// FORTES: Claim 3
      __MAP_FORTES_assert((intptr_t)i >= 0, "[Failed]\n VIOLATED PROPERTY: Claim 3 \n \t Location at original code in line: 31 \n \t Comments:   array `a' lower bound");
// FORTES: Claim 4
      __MAP_FORTES_assert((intptr_t)i < 5, "[Failed]\n VIOLATED PROPERTY: Claim 4 \n \t Location at original code in line: 31 \n \t Comments:   array `a' upper bound");
// FORTES: Claim 5
      __MAP_FORTES_assert((intptr_t)1 + i >= 0, "[Failed]\n VIOLATED PROPERTY: Claim 5 \n \t Location at original code in line: 31 \n \t Comments:   array `a' lower bound");
// FORTES: Claim 6
      __MAP_FORTES_assert((intptr_t)1 + i < 5, "[Failed]\n VIOLATED PROPERTY: Claim 6 \n \t Location at original code in line: 31 \n \t Comments:   array `a' upper bound");
      a[i]         = a[i + 1] + i;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(a), (void *)(intptr_t)(NULL), 2, 0, 0, 0, "int", 31); /** by FORTES **/

// FORTES: Claim 7
      __MAP_FORTES_assert((intptr_t)i >= 0, "[Failed]\n VIOLATED PROPERTY: Claim 7 \n \t Location at original code in line: 32 \n \t Comments:   array `a' lower bound");
// FORTES: Claim 8
      __MAP_FORTES_assert((intptr_t)i < 5, "[Failed]\n VIOLATED PROPERTY: Claim 8 \n \t Location at original code in line: 32 \n \t Comments:   array `a' upper bound");
      temp         = a[i] * (i + 1);
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(temp), (void *)(intptr_t)(NULL), 2, 0, 0, 0, "int", 32); /** by FORTES **/

      for (j = 0; j < temp; j++)
      {
// FORTES: Claim 9
         __MAP_FORTES_assert((intptr_t)j >= 0, "[Failed]\n VIOLATED PROPERTY: Claim 9 \n \t Location at original code in line: 34 \n \t Comments:   array `b' lower bound");
// FORTES: Claim 10
         __MAP_FORTES_assert((intptr_t)j < 6, "[Failed]\n VIOLATED PROPERTY: Claim 10 \n \t Location at original code in line: 34 \n \t Comments:   array `b' upper bound");
// FORTES: Claim 11
         __MAP_FORTES_assert((intptr_t)i - 1 >= 0, "[Failed]\n VIOLATED PROPERTY: Claim 11 \n \t Location at original code in line: 34 \n \t Comments:   array `b' lower bound");
// FORTES: Claim 12
         __MAP_FORTES_assert((intptr_t)i - 1 < 6, "[Failed]\n VIOLATED PROPERTY: Claim 12 \n \t Location at original code in line: 34 \n \t Comments:   array `b' upper bound");
         b[j]         = b[i - 1] + (temp * 2);
         list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(b), (void *)(intptr_t)(NULL), 2, 0, 0, 0, "int", 34); /** by FORTES **/
      }
   }
   if (i > 0)
   {
      assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 49)); /** by FORTES **/
      exit(0);
      assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 50)); /** by FORTES **/
      abort();
   }
   else
   {
      double df = 0.0;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(df), (void *)(intptr_t)(NULL), 2, 0, 0, 0, "double", 42); /** by FORTES **/

      ka           = 1234567;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(ka), (void *)(intptr_t)(NULL), 2, 0, 0, 0, "unsigned int", 43); /** by FORTES **/
   }
   float te = 1.5;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(te), (void *)(intptr_t)(NULL), 2, 0, 0, 0, "float", 46); /** by FORTES **/

   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 59));                                                                 /** by FORTES **/
   return(0);

   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 27)); /** by FORTES **/
}
