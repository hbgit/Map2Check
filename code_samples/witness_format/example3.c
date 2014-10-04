extern void __VERIFIER_error(void);                                             

int failure() {
    __VERIFIER_error();
    return 1;
}

int main() {
    int a = 1;
    return a && failure();
}  
