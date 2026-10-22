#include <stdlib.h>
#include <string.h>
int main(void) {
  char *p = malloc(8);
  char q[8] = {0};
  if (p == 0) return 0;
  memset(p, 0, 16);
  memcpy(q, p, 8);
  memmove(p, q, 8);
  free(p);
  return 0;
}
