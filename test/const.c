const int globalConst;
int globalVar;

int testStatic() {
    static int a = 9;
    a++;
    return a;
}


int main() {
    const int localConst;
    const int constVector[5];

    int normalVector[] = {1,2,3};
    int* normalArray = {1,2,3};

    char stringVector[] = {'a', 'b', 'c' };
    char* stringPointer = "abc";
    return 0;
}
