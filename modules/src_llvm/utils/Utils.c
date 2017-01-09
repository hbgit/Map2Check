#include <stdio.h>

void map2check_pointer(void* x, const char* name){
  // printf("Hello %s\n", name);
  // klee_make_symbolic(x, sizeof(*x), name);
}

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
