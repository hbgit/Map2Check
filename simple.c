#include <stdio.h>

int main() {
  int* A = malloc(sizeof(int));
  coiso();

  int x = __VERIFIER_nondet_int();

  if(x) {
    x = __VERIFIER_nondet_int();
    if (!x) {
      free(A);
    }
  }

  free(A);
}

int coiso() {
  int* B = malloc(sizeof(int));
  free(B);
}
