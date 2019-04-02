#include "../header/Map2CheckFunctions.h"
#include "../header/NonDetGenerator.h"
#include "../header/NonDetLog.h"

extern int __map2check_main__();

int main() { return __map2check_main__(); }

void nondet_init() { nondet_log_init(); }
void nondet_destroy() { nondet_log_destroy(); }
void nondet_generate_aux_witness_files() {
  nondet_log_to_file(map2check_nondet_get_log());
}

extern void klee_assume(int);
void nondet_assume(int expr) { klee_assume(expr); }

extern void klee_make_symbolic(void *addr, size_t nbytes, const char *name);
#define MAP2CHECK_NON_DET_GENERATOR(type)                           \
  type map2check_non_det_##type() {                                 \
    type non_det;                                                   \
    klee_make_symbolic(&non_det, sizeof(non_det), "non_det_#type"); \
    return non_det;                                                 \
  }

// TODO: this should be dynamic
char *map2check_non_det_pchar() {
  unsigned length = map2check_non_det_unsigned();
  if (length == 0) return NULL;
  char string[length];
  unsigned i = 0;
  for (i = 0; i < (length - 1); i++) {
    string[i] = map2check_non_det_char();
  }
  string[i] = '\0';
  return string;
}

// Using default generator for:
MAP2CHECK_NON_DET_GENERATOR(int)
MAP2CHECK_NON_DET_GENERATOR(char)
MAP2CHECK_NON_DET_GENERATOR(pointer)
MAP2CHECK_NON_DET_GENERATOR(ushort)
MAP2CHECK_NON_DET_GENERATOR(long)
MAP2CHECK_NON_DET_GENERATOR(unsigned)
MAP2CHECK_NON_DET_GENERATOR(ulong)
MAP2CHECK_NON_DET_GENERATOR(bool)
MAP2CHECK_NON_DET_GENERATOR(uchar)
MAP2CHECK_NON_DET_GENERATOR(size_t)
#ifndef __INTELLISENSE__
MAP2CHECK_NON_DET_GENERATOR(loff_t)
#endif
MAP2CHECK_NON_DET_GENERATOR(sector_t)
MAP2CHECK_NON_DET_GENERATOR(uint)
