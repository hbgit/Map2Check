
int teste(const int coiso) {
//    coiso++;
    printf("Argument address: %p\n", &coiso);
    printf("Points To: %d\n", coiso);
    return 0;
}

//int no_arg(int b) {
//    printf("TESTE 2: %p\n", &b);
//    printf("TESTE 3: %d\n", b);
//    return 2;
//}

void no_arg_void() {

}



int main() {
    int a = 7;
    printf("Address of var: %p\n", &a);
    teste(a);
//    no_arg(a);
}
