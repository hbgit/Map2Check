#include <assert.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include <stdio.h>                      /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/

#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);

char *cstrncat(char *s1, const char *s2, int n)
{
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(s1), (void *)(intptr_t)(s1), "s1", 1, 0, 0, 0, "char", 5); /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(s2), (void *)(intptr_t)(s2), "s2", 1, 0, 0, 0, "char", 5); /** by FORTES **/

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(n), (void *)(intptr_t)(NULL), "n", 1, 0, 0, 0, "int", 5);  /** by FORTES **/

   char *s = s1;
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(s), (void *)(intptr_t)(s), "s", 1, 0, 0, 0, "char", 7); /** by FORTES **/

// FORTES: Claim 1
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&(s), (void *)(intptr_t)(s))), "[Failed]\n VIOLATED PROPERTY: Claim 1 \n \t Location at original code in line: 9 \n \t Comments:   dereference failure: invalid pointer");
   while ((*s) != '\0')
   {
      s++;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(s), (void *)(intptr_t)(s), "s", 1, 0, 0, 0, "char", 10); /** by FORTES **/
   }
// FORTES: Claim 2
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&(s), (void *)(intptr_t)(s))), "[Failed]\n VIOLATED PROPERTY: Claim 2 \n \t Location at original code in line: 16 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 3
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 3 \n \t Location at original code in line: 16 \n \t Comments:   dereference failure: invalid pointer");
// FORTES: Claim 4
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&(s), (void *)(intptr_t)(s))), "[Failed]\n VIOLATED PROPERTY: Claim 4 \n \t Location at original code in line: 16 \n \t Comments:   dereference failure: invalid pointer");
   while ((n != 0) && ((*s = *(s2++)) != '\0'))
   {
      n--;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(n), (void *)(intptr_t)(n), "n", 1, 0, 0, 0, "int", 17); /** by FORTES **/

      s++;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(s), (void *)(intptr_t)(s), "s", 1, 0, 0, 0, "char", 18); /** by FORTES **/
   }
// FORTES: Claim 5
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&(s), (void *)(intptr_t)(s))), "[Failed]\n VIOLATED PROPERTY: Claim 5 \n \t Location at original code in line: 20 \n \t Comments:   dereference failure: invalid pointer");
   if ((*s) != '\0')
   {
// FORTES: Claim 6
      __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&(s), (void *)(intptr_t)(s))), "[Failed]\n VIOLATED PROPERTY: Claim 6 \n \t Location at original code in line: 21 \n \t Comments:   dereference failure: invalid pointer");
      *s           = '\0';
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(s), (void *)(intptr_t)(s), "s", 1, 0, 0, 0, "char", 21); /** by FORTES **/
   }
   return(s1);
}


int main()
{
   int length1 = __VERIFIER_nondet_int();

   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(length1), (void *)(intptr_t)(NULL), "length1", 2, 0, 0, 0, "int", 26); /** by FORTES **/

   int length2 = __VERIFIER_nondet_int();
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(length2), (void *)(intptr_t)(NULL), "length2", 2, 0, 0, 0, "int", 27); /** by FORTES **/

   int n = __VERIFIER_nondet_int();
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(n), (void *)(intptr_t)(NULL), "n", 2, 0, 0, 0, "int", 28); /** by FORTES **/

   if (length1 < 1)
   {
      length1      = 1;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(length1), (void *)(intptr_t)(NULL), "length1", 2, 0, 0, 0, "int", 30); /** by FORTES **/
   }
   if (length2 < 1)
   {
      length2      = 1;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(length2), (void *)(intptr_t)(NULL), "length2", 2, 0, 0, 0, "int", 33); /** by FORTES **/
   }
   if (n < 1)
   {
      n            = 1;
      list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(n), (void *)(intptr_t)(NULL), "n", 2, 0, 0, 0, "int", 36); /** by FORTES **/
   }
   if ((length1 < n) || ((length1 - n) < length2))
   {
      assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 38)); /** by FORTES **/
      return(0);
   }
   char *nondetString1 = (char *)alloca(length1 * (sizeof(char)));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(nondetString1), (void *)(intptr_t)(nondetString1), "nondetString1", 2, 1, 0, 0, "char", 39); /** by FORTES **/

   char *nondetString2 = (char *)alloca(length2 * (sizeof(char)));
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(nondetString2), (void *)(intptr_t)(nondetString2), "nondetString2", 2, 1, 0, 0, "char", 40); /** by FORTES **/

// FORTES: Claim 7
   __MAP_FORTES_assert((1), "[Failed]\n VIOLATED PROPERTY: Claim 7 \n \t Location at original code in line: 41 \n \t Comments:   dereference failure: invalid pointer");
   nondetString1[(length1 - n) - 1] = '\0';
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(nondetString1[(length1 - n) - 1]), (void *)(intptr_t)(nondetString1[(length1 - n) - 1]), "nondetString1[(length1 - n) - 1]", 2, 0, 0, 0, "char", 41); /** by FORTES **/

// FORTES: Claim 8
   __MAP_FORTES_assert(!(IS_VALID_POINTER_FORTES(list_LOG_mcf, (void *)&(*(length2 - 1 + nondetString2)), (void *)(intptr_t)(*(length2 - 1 + nondetString2)))), "[Failed]\n VIOLATED PROPERTY: Claim 8 \n \t Location at original code in line: 42 \n \t Comments:   dereference failure: invalid pointer");
   nondetString2[length2 - 1] = '\0';
   list_LOG_mcf = mark_map_FORTES(list_LOG_mcf, (void *)&(nondetString2[length2 - 1]), (void *)(intptr_t)(nondetString2[length2 - 1]), "nondetString2[length2 - 1]", 2, 0, 0, 0, "char", 42); /** by FORTES **/

   cstrncat(nondetString1, nondetString2, n);
   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 44)); /** by FORTES **/
   return(0);

   assert(CHECK_MEMORY_LEAK(list_LOG_mcf, 0, 44)); /** by FORTES **/
}
