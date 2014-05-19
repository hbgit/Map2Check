#include <stdio.h>
#include <stdint.h>

void * G_void_ptr;
int * G_int_ptr;

typedef struct{
    int a;
    int * b;
}tStruct;


//foo(int * k, tStruct * l){
foo(tStruct * l){
//foo(int * k){
//foo(){
    void * Lfoo_void_ptr;
    printf("Lfoo_void_ptr: %p -- %p \n",&Lfoo_void_ptr, (void*)(intptr_t)Lfoo_void_ptr);
    
    //int * Lfoo_int_ptr;
    //printf("Lfoo_int_ptr: %p -- %p \n",&Lfoo_int_ptr, (void*)(intptr_t)Lfoo_int_ptr);
    //tStruct * Z;
}


main_count(){
    return 0;
}


//int count(){
////int count(int * z){
    //int i = 10;
    //int k;
    //while(i < 9){
        //k = i;
    //}
    
    //return 1;
//}


main(int argc, char **argv){
    
    //foo();
    //void * L_void_ptr;
    //printf("MAIN -> L_void_ptr: %p -- %p \n",&L_void_ptr, (void*)(intptr_t)L_void_ptr);
    if(1){
        int * u;
    }    
    
    int * L_int_ptr;
    printf("L_int_ptr: %p -- %p \n",&L_int_ptr, (void*)(intptr_t)L_int_ptr);
    int * ui;
   
    /********
    tStruct * l;
    foo(l);
    * 
    * <<< Gera lixo em ponteiro não inicializado mesmo que esteja no topo da pilha
    * ********/ 
    
    ///** Aqui o topo da pilha já esta cheio == Lixo no ponteiro
     //* 
    //if(count()){
        //void * L_void_ptr;
        //printf("L_void_ptr: %p -- %p \n",&L_void_ptr, (void*)(intptr_t)L_void_ptr);
    //}*/
    
    //printf("\n");
    //printf("MAIN -> L_void_ptr: %p -- %p \n",&L_void_ptr, (void*)(intptr_t)L_void_ptr);
    
    //int * L_int_ptr; //lixo
    
    //count(L_int_ptr);
    
    //tStruct * l;
    ////void * L_void_ptr;
    
    
    
    ////foo(L_int_ptr,l);
    ////foo(L_int_ptr);
    //foo();
    
    //printf("G_int_ptr: %p -- %p \n",&G_int_ptr, (void*)(intptr_t)G_int_ptr);
    //printf("G_void_ptr: %p -- %p \n",&G_void_ptr, (void*)(intptr_t)G_void_ptr);
    
    //printf("\n");
    
    //printf("L_int_ptr: %p -- %p \n",&L_int_ptr, (void*)(intptr_t)L_int_ptr);
    ////printf("MAIN -> L_void_ptr: %p -- %p \n",&L_void_ptr, (void*)(intptr_t)L_void_ptr);
}
