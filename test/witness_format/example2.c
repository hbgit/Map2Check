//This program is safe and yields no error path.

extern void __VERIFIER_error(void);

int main() {
    int a = 1, b = 0, c;

    c = a || b;

    if (!c) {
        __VERIFIER_error();
        return 1;
    }

    return 0;
}    
