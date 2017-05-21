int main() {
    int x[2];
    void* pointer = (void*) x;

    pointer += 2;
    int result = *(int*) pointer;

    return 0;
}
