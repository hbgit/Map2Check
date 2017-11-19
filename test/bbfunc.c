//#include <stdio.h>
// clang -emit-llvm -c bbfunc.c
// opt -constprop < bbfunc.bc > newbbfunc.bc
// llvm-dis newbbfunc.bc
// cat newbbfunc.ll

// CFG view
// clang -emit-llvm -c file.c
// llvm-dis file.bc
// opt -cfg-dot file.ll
// dot -Tpng *.dot > graph.png

int main()
{
    int a = 1;
    if(a < 11)
    {
        a++;
        int b = a;
    }
    
    if(a > 11)
    {
        a--;
    }

    return 0;
}
