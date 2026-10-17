#include <stdlib.h>
int g = 7;
int *make(void) { return malloc(sizeof(int)); }
int main(void) {
  int n = 3;
  int *p = malloc(4 * sizeof(int));
  int *q = calloc(2, sizeof(int));
  p = realloc(p, 8 * sizeof(int));
  int arr[5];
  int vla[n];
  void *al = NULL;
  posix_memalign(&al, 16, 64);
  arr[0] = *q;
  vla[0] = g;
  free(p);
  free(make());
  free(al);
  free(q);
  return arr[0] + vla[0];
}
