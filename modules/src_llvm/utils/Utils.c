#include <stdio.h>

int map2check_non_det_int() {
  int non_det;
  klee_make_symbolic(&non_det, sizeof(non_det), "non_det_int");
  return non_det;
}

void map2check_malloc(void* ptr, int size) {
  printf("Allocated %p\n", ptr);
}

void map2check_free(void* ptr) {
  printf("Freed %p\n", ptr);
}
