#include <stdio.h>                      /** by FORTES **/
#include <assert.h>                     /** by FORTES **/
#include <stdlib.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/
#include <CUnit/Basic.h>                /** by FORTES **/
#include "check_safety_memory_FORTES.h" /** by FORTES **/


/* Pointer to the file used by the tests. */
static FILE* temp_file = NULL;

/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int init_suite1(void) {
    if (NULL == (temp_file = fopen("temp.txt", "w+"))) {
        return -1;
    } else {
        return 0;
    }
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1(void) {
    if (0 != fclose(temp_file)) {
        return -1;
    } else {
        temp_file = NULL;
        return 0;
    }
}



/****************************************************
 * Code Block -> Test
 * **************************************************/
/************** Variables ***************************/
int *a, *b;
int n;
/************** End Variables ***************************/

#define BLOCK_SIZE 128

/************** Functions ***************************/
void foo ()
{
  int i;
  for (i = 0; i < n; i++){    
    CU_ASSERT(1);
    CU_ASSERT(1); 
    a[i] = -1; //Claim 1 Claim 2:    
    list_DIN_OBJT_FORTES = mark_map_FORTES(list_DIN_OBJT_FORTES, (void *)&a[i], (void *)(intptr_t)NULL, 1, 0, 0, 12);          /** by FORTES **/
  }
  for (i = 0; i < BLOCK_SIZE - 1; i++){    
    CU_ASSERT(1);
    CU_ASSERT(1);
    b[i] = -1; //Claim 3 Claim 4
    list_DIN_OBJT_FORTES = mark_map_FORTES(list_DIN_OBJT_FORTES, (void *)&b[i], (void *)(intptr_t)NULL, 1, 0, 0, 14);          /** by FORTES **/
  }
}
/************** End Functions ***************************/

/************** main() == testClaims ***************************/
void testClaims(void) {
  
  n = BLOCK_SIZE;  
  a = malloc (n * sizeof(*a));
  list_DIN_OBJT_FORTES = mark_map_FORTES(list_DIN_OBJT_FORTES, (void *)&a, (void *)(intptr_t)a, 2, 1, 0, 20);          /** by FORTES **/
  
  b = malloc (n * sizeof(*b));
  list_DIN_OBJT_FORTES = mark_map_FORTES(list_DIN_OBJT_FORTES, (void *)&b, (void *)(intptr_t)b, 2, 1, 0, 21);          /** by FORTES **/
  
  *b++ = 0;
  list_DIN_OBJT_FORTES = mark_map_FORTES(list_DIN_OBJT_FORTES, (void *)&b, (void *)(intptr_t)b, 2, 0, 0, 22);          /** by FORTES **/
  
  foo ();
  //Claim 5 Claim 6
  CU_ASSERT(1);
  CU_ASSERT(1);
  if (b[-1])
  {     
    CU_ASSERT_FATAL(INVALID_FREE(list_DIN_OBJT_FORTES, (void *)(intptr_t)a,25));                                     /** CLAIM by FORTES **/    
    free(a); 
    list_DIN_OBJT_FORTES = mark_map_FORTES(list_DIN_OBJT_FORTES, (void *)&a, (void *)(intptr_t)a, 2, 0, 0, 25);          /** by FORTES **/
    
    
    //Claim 7 Claim 8  
    CU_ASSERT(1);
    CU_ASSERT(1);
    CU_ASSERT_FATAL(INVALID_FREE(list_DIN_OBJT_FORTES, (void *)(intptr_t)b,25));                                     /** CLAIM by FORTES **/    
    free(b); 
    list_DIN_OBJT_FORTES = mark_map_FORTES(list_DIN_OBJT_FORTES, (void *)&b, (void *)(intptr_t)b, 2, 0, 0, 25);          /** by FORTES **/
    
  } /* invalid free (b was iterated) */
  else
  {   
      CU_ASSERT_FATAL(INVALID_FREE(list_DIN_OBJT_FORTES, (void *)(intptr_t)a,27));                                     /** CLAIM by FORTES **/    
      free(a); 
      list_DIN_OBJT_FORTES = mark_map_FORTES(list_DIN_OBJT_FORTES, (void *)&a, (void *)(intptr_t)a, 2, 0, 0, 27);          /** by FORTES **/
      //Claim 9 Claim 10 
      CU_ASSERT(1);
      CU_ASSERT(1);   
      CU_ASSERT_FATAL(INVALID_FREE(list_DIN_OBJT_FORTES, (void *)(intptr_t)b,27));                                     /** CLAIM by FORTES **/
      free(b); 
      list_DIN_OBJT_FORTES = mark_map_FORTES(list_DIN_OBJT_FORTES, (void *)&b, (void *)(intptr_t)b, 2, 0, 0, 27);          /** by FORTES **/
  } /* ditto */
  
}

void MAP_GB_VAR_FORTES(){
    list_DIN_OBJT_FORTES = mark_map_FORTES(list_DIN_OBJT_FORTES, (void *)&n, (void *)(intptr_t)NULL, 0, 0, 0, 4);          /** by FORTES **/    
}
/************** End -> main() == testClaims ***************************/
/****************************************************
 * End Code Block -> Test
 * **************************************************/
 

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main() {
    MAP_GB_VAR_FORTES(); /** by FORTES **/
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* add a suite to the registry */
    pSuite = CU_add_suite("mf_960521-1_false-valid-free", init_suite1, clean_suite1);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
    if ((NULL == CU_add_test(pSuite, "testClaims", testClaims))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}

