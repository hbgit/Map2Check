#include <stdio.h>
#include <string.h>

typedef unsigned char BYTE;


void print_by_type(void *addr, char *type, int isptr){
    
    if(strcmp(type,"none") == 0){
        printf("none");
    }
    else if(strcmp(type,"int") == 0){ 
        printf("%p \n", addr);       
        printf("int %d", *((int*)addr));
    }
    else if(strcmp(type,"unsigned int") == 0){
        printf("%u", *((unsigned int*)addr));
    }
    else if(strcmp(type,"short") == 0 || strcmp(type,"short int") == 0){
        printf("%hi", *((short*)addr)); 
    }
    else if(strcmp(type,"unsigned short") == 0 || strcmp(type,"unsigned short int") == 0){
        printf("%u", *((unsigned short*)addr));
    }
    else if(strcmp(type,"long") == 0 || strcmp(type,"long int") == 0){
        printf("%li", *((long int*)addr));
    }
    else if(strcmp(type,"unsigned long") == 0 || strcmp(type,"unsigned long int") == 0){
        printf("%lu", *((unsigned long*)addr));
    }
    else if(strcmp(type,"float") == 0){
        printf("%f", *((float*)addr));
    }
    else if(strcmp(type,"double") == 0){
        printf("%f", *((double*)addr));
    }
    else if(strcmp(type,"long double") == 0){
        printf("%llf", *((long double*)addr));
    }
    else if(strcmp(type,"char") == 0 || strcmp(type,"signed char") == 0){
        if(isptr == 0){
            printf("%c", *((char*)addr));
        }else{
            printf("%s", *((char**)addr));
        }
    }
    else if(strcmp(type,"unsigned char") == 0){
        printf("%c", *((unsigned char*)addr));
    }        
    
    else{
        printf("ERROR");
    }
    
    
}

main(){
    
    
    
    BYTE b1 = 'c';
    void *ptr;
    
    
    //ptr = &b1;
    //printf("%c \n", b1);
    
    //char *t = "teste";
    
    //printf("%s ; ", t);
    //print_by_type(ptr,"none");
    //printf("; %s ; \n", t);
    
    //test cases
    int v = 2147483648;
    printf("%p \n", &v);  
    ptr = (void*)&v;
    print_by_type(ptr,"int",0);
    
    printf("----- %d \n", v);
    
    printf("\n");    
    unsigned int v1 = 2147483648;
    ptr = (void*)&v1;    
    print_by_type(ptr,"unsigned int",0);
    printf("\n");
    
    printf("\n");    
    short v2 = -32766;    
    ptr = (void*)&v2;     
    print_by_type(ptr,"short",0);
    printf("\n");
    
    
    printf("\n");    
    short int v3 = -32766;    
    ptr = (void*)&v3;     
    print_by_type(ptr,"short int",0);
    printf("\n");
    
    printf("\n");    
    unsigned short v4 = 32766;    
    ptr = (void*)&v4;     
    print_by_type(ptr,"unsigned short",0);
    printf("\n");
    
    printf("\n");    
    long v5 = -2147483648;    
    ptr = (void*)&v5;     
    print_by_type(ptr,"long",0);
    printf("\n");
    
    printf("\n");    
    long int v6 = -2147483648;    
    ptr = (void*)&v6;     
    print_by_type(ptr,"long int",0);
    printf("\n");
    
    printf("\n");    
    unsigned long v7 = 2147483648;    
    ptr = (void*)&v7;     
    print_by_type(ptr,"unsigned long",0);
    printf("\n");
    
    printf("\n");    
    unsigned long int v8 = 2147483648;    
    ptr = (void*)&v8;     
    print_by_type(ptr,"unsigned long int",0);
    printf("\n");
    
    printf("\n");    
    float v9 = 3.4E+38;    
    ptr = (void*)&v9;     
    print_by_type(ptr,"float",0);
    printf("\n");
    
    
    printf("\n");    
    double v10 = 1.7E+308;    
    ptr = (void*)&v10;     
    print_by_type(ptr,"double",0);
    printf("\n");
    
    printf("\n");    
    long double v11 = 1.7E+308;    
    ptr = (void*)&v11;     
    print_by_type(ptr,"long double",0);
    printf("\n");
    
    printf("\n");    
    char v12 = 'c';    
    ptr = (void*)&v12;     
    print_by_type(ptr,"char",0);
    printf("\n");
    
    printf("\n");    
    char v13 = 'c';    
    ptr = (void*)&v13;     
    print_by_type(ptr,"signed char",0);
    printf("\n");
    
    
    printf("\n");    
    char v14 = 'c';    
    ptr = (void*)&v14;     
    print_by_type(ptr,"unsigned char",0);
    printf("\n");
    
    printf("\n");    
    char *v15 = "casa";    
    ptr = (void*)&v15;     
    print_by_type(ptr,"char",1);
    printf("\n");
    
    
    
    
    
    
    
}
