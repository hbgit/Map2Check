#include <stdio.h>
#include <stdlib.h>
#include "myH.h"

/** 
 * Oracle to Map by variable ocurences
 * -> Num. Malloc            : 5
 * -> Num. Free              : 4  
 * -> Num. Total Obj. Mapped : 43
 * -> Lines to map by objects: [25,27,29,31,33,37,37,37,42,43,44,46,50,52,55,55,55,58,62,68,69,75,77,86,89,95,102,106,111,111,115,115,117,119,122,128,130,134,140,142,144,146,148]
 * */


typedef int *int_ptr;


typedef struct
{
   void *lo;
   void *hi;
   int *ke;
} TData;


int globla_A = 12;

int global_B;

int myArray[4] = {1,2,3,4};

int *ptrInt = 12;

int *gb_y = &globla_A;

int *yu;

int *kri = NULL,jri = 0,ori; 

/**
 * Function
 * */
zeta(TData *pdata){
    pdata->lo = malloc(16);
    *(pdata->ke) = globla_A = global_B; //CK list
    
    int *p = malloc(3);
    
    int *myT;
    
    myT = malloc(sizeof(int));
    
    *myT = globla_A;
} 

delta(int k, int je, char *ex){
	
	char *j;
	j = (char*) malloc(5 * sizeof(char)); 	
		
	k++;
    
    gb_y = &k;
    
    je = 0;
	
	while(je == 0){
        if(k < 0){
            free(j);
            j = NULL;
        }
        je = 1;
        free(yu); 
    }
    
    free(j);	
    
    ex = j;
    
    switch( je ) 
    {
        case 0 :
            je = 3;
            break;
        case 1 :
            k--;
            gb_y = NULL;
            break;
        case 2 :
            j = NULL;
            break;
    }
    
}

int **gb_de = NULL;


main(){
    
    //Checkout struct Funcionario empregado1;
    
    TData A; 
    
    int_ptr i_typedef;
    
    i_typedef = NULL;
    
    char_ptr d_char;
    
    		
	int int_value; d_char = NULL;
			
	int *p;	
    
    p = i_typedef = gb_y; //CK list
	
	p = &int_value;
		
	int c = 2;
		
	int *q;
	q = (int*) malloc(5 * sizeof(int)); 
    
    char * ooo;
		
	delta(c,c,ooo);	
		
	p = &c;
    
    gb_y = p;
		
	if(globla_A ==  12){
		while( c == 1){
            free(q);
            c--;
        }        
	}
    
    if(globla_A ==  1){
        int t = 1;
        globla_A = t;
        gb_y = NULL;
    }else{
        int w = 0;
        while(w = 0){
            gb_y = NULL;
            w++;
            free(gb_y);
        }
        
    }
		
	int *error;
	
}
