extern void __VERIFIER_error(void);

int main() {
    int a = 1;
    int c = 1 || (a = 0);
    if (a) { __VERIFIER_error(); return 1; }
    //int k;
    return 0;
}    
