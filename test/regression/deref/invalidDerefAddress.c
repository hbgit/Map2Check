
int main() {
    int vector[3]; //If start address is 0x10, the last one should be 0x1B (Assuming 4byte integer)

    void* derefPointer = &vector[2]; //Address should be 0x18

    derefPointer++; //Address should be 0x19

    int* errorPointer = (int*) derefPointer;
    int error = *errorPointer; //Here is the deref error

    return 0;
}
