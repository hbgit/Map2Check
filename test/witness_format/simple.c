// #include <stdio.h>

int main() {
  int* A = malloc(sizeof(int));

  int x = __VERIFIER_nondet_int();

  if(x) {
    x = __VERIFIER_nondet_int();
    if (x) {
      free(A);
    }
  }

  free(A);
  return 0;
}
