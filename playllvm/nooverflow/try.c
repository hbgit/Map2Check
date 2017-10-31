#include <stdio.h>

main()
{
    
    unsigned int ui = 4294967294;
    int i = ui;
    
    printf("%u\n", ui);
    printf("%d\n", i);
    
    unsigned int v = -4;
    if((-4 - v) > -1)
    {
        printf("Maior \n");
    }else{
        printf("Menor \n");
    }


    if(0 < -1){
        printf("ZERO \n");
    }
}
