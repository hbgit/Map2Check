int main() {
    int* A = malloc(sizeof(int));
    int* B = A;
    int* C = A;

    free (A);
    return 0;
}