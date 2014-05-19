#include <stdlib.h>
#include <stdlib.h>                     /** by FORTES **/
#include <stdint.h>                     /** by FORTES **/

int *a, *b;
int n;


#define BLOCK_SIZE 2

void foo ()
{
  int i;
  printf("foo: %p ---- %p \n", (void *)&i, (void *)(intptr_t)NULL);
  for (i = 0; i < n; i++){
    a[i] = -1;
    printf("global: %p ---- %p \n", (void *)&a[i], (void *)(intptr_t)a[i]);
    //*(a+i) = -1;
  }
  for (i = 0; i < BLOCK_SIZE - 1; i++){
    b[i] = -1;
    printf("global: %p ---- %p \n", (void *)&b[i], (void *)(intptr_t)b[i]);
  }
}

int main ()
{
  printf("global %p ---- %p \n", (void *)&n, (void *)(intptr_t)NULL);
  n = BLOCK_SIZE;
  a = malloc (n * sizeof(*a));
  printf("global malloc: %p ---- %p \n", (void *)&a, (void *)(intptr_t)a);
  b = malloc (n * sizeof(*b));
  printf("global malloc B: %p ---- %p \n", (void *)&b, (void *)(intptr_t)b);
  *b++ = 0;
  printf("global BUG: %p ---- %p \n", (void *)&b, (void *)(intptr_t)b);
  foo ();
  if (b[-1])
  { free(a); free(b); 
    printf("global Free: %p ---- %p \n", (void *)&a, (void *)(intptr_t)a);
    printf("global Free: %p ---- %p \n", (void *)&b, (void *)(intptr_t)b);
  } /* invalid free (b was iterated) */
  else
  { 
      printf("global Free: %p ---- %p \n", (void *)&a, (void *)(intptr_t)a);
      printf("global Free: %p ---- %p \n", (void *)&b, (void *)(intptr_t)b);
      free(a); free(b); 
    
  } /* ditto */

  return 0;
}
