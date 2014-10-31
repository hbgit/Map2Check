#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


/**
 * [TO DO] Add tratamento de exceção
 **/


/*******************************************************************/
union Data
{
   int dataInt;
   float dataFloat;
   char  *dataChar;
};


typedef struct obj {
  void  *adresse_mem_map_FORTES;
  void  *block_MEM_Point_FORTES;

  char  *map_namevar_FORTES;
  int   map_ID_func_FORTES;
  int   map_is_DYNAM_FORTES; 
  int   map_setFree_FORTES;
  int   map_isUnion_FORTES;
  char  *map_typevar_FORTES;
  int   map_linePreCode_FORTES;
  
  struct obj *next_item_FORTES; 
} LIST_DYN_OBJ_FORTES;

//Global counter for items in the list to map
int count_all_list_FORTES=0;
int line_of_the_last_use = 0;

int FLAG_DEBUG = 0;

//Counter-example
LIST_DYN_OBJ_FORTES *list_CE_trace;
FILE *gb_actualfile;

int FLAG_to_IS_VALID_POINTER_FORTES = 0;


/*******************************************************************/
// Function to print the variable value from memory address based
// on the variable type
/*******************************************************************/
void print_by_type(void *addr, char *type, int isptr, int iswritefile){

    if(strcmp(type,"none") == 0){
        if(iswritefile){
            fprintf(gb_actualfile,"| %4s\t\t", "none");
        }else{
            printf("none");
        }

    }
    else if(strcmp(type,"int") == 0){
        if(iswritefile){
            fprintf(gb_actualfile,"| %4d\t\t", *((int*)addr));
        }else{
            printf("%12d", *((int*)addr));
        }
    }
    else if(strcmp(type,"unsigned int") == 0){
        if(iswritefile){
            fprintf(gb_actualfile,"| %4u\t\t", *((unsigned int*)addr));
        }else{
            printf("%12u", *((unsigned int*)addr));
        }
    }
    else if(strcmp(type,"short") == 0 || strcmp(type,"short int") == 0){
        if(iswritefile){
            fprintf(gb_actualfile,"| %4hi\t\t", *((short*)addr));
        }else{
            printf("%12hi", *((short*)addr));
        }
    }
    else if(strcmp(type,"unsigned short") == 0 || strcmp(type,"unsigned short int") == 0){
        if(iswritefile){
            fprintf(gb_actualfile,"| %4u\t\t", *((unsigned short*)addr));
        }else{
            printf("%12u", *((unsigned short*)addr));
        }
    }
    else if(strcmp(type,"long") == 0 || strcmp(type,"long int") == 0){
        if(iswritefile){
            fprintf(gb_actualfile,"| %4li\t\t", *((long int*)addr));
        }else{
            printf("%12li", *((long int*)addr));
        }
    }
    else if(strcmp(type,"unsigned long") == 0 || strcmp(type,"unsigned long int") == 0){
        if(iswritefile){
            fprintf(gb_actualfile,"| %4lu\t\t", *((unsigned long*)addr));
        }else{
            printf("%12lu", *((unsigned long*)addr));
        }
    }
    else if(strcmp(type,"float") == 0){
        if(iswritefile){
            fprintf(gb_actualfile,"| %4f\t\t", *((float*)addr));
        }else{
            printf("%12f", *((float*)addr));
        }
    }
    else if(strcmp(type,"double") == 0){
        if(iswritefile){
            fprintf(gb_actualfile,"| %4f\t\t", *((double*)addr));
        }else{
            printf("%12f", *((double*)addr));
        }
    }
    else if(strcmp(type,"long double") == 0){
        if(iswritefile){
            fprintf(gb_actualfile,"| %4llf\t\t", *((long double*)addr));
        }else{
            printf("%12llf", *((long double*)addr));
        }
    }
    else if(strcmp(type,"char") == 0 || strcmp(type,"signed char") == 0){
        if(isptr == 0){
            if(iswritefile){
                if(*((char*)addr) == 0){
                    fprintf(gb_actualfile,"| %4c\t\t", '0');
                }else{
                    fprintf(gb_actualfile,"| %4c\t\t", *((char*)addr));
                }
            }else{
                if(*((char*)addr) == 0){
                    fprintf(gb_actualfile,"%12c", '0');
                }else{
                    printf("%12c", *((char*)addr));
                }

            }
        }else{
            if(iswritefile){
                if(*((char**)addr) == 0){
                    fprintf(gb_actualfile,"| %4s\t\t", '0');
                }else{
                    fprintf(gb_actualfile,"| %4s\t\t", *((char**)addr));
                }
            }else{
                if(*((char**)addr) == 0){
                    fprintf(gb_actualfile,"%12s\t\t", '0');
                }else{
                    printf("%12s", *((char**)addr));
                }
            }
        }
    }
    else if(strcmp(type,"unsigned char") == 0){
        if(iswritefile){
            if(*((unsigned char*)addr) == 0){
                fprintf(gb_actualfile,"| %4c\t\t", '0');
            }else{
                fprintf(gb_actualfile,"| %4c\t\t", *((unsigned char*)addr));
            }

        }else{
            if(*((unsigned char*)addr) == 0){
                fprintf(gb_actualfile,"| %12c\t\t", '0');
            }else{
                printf("%12c", *((unsigned char*)addr));
            }
        }
    }

    else{
        if(iswritefile){
            fprintf(gb_actualfile,"| %4s\t\t", "--");
        }else{
            printf("--");
        }
    }


}


LIST_DYN_OBJ_FORTES *list_LOG_complete_TRACE = NULL;


/*******************************************************************/
// Function to write a log for each action in the map list
/*******************************************************************/
void write_logfile_maplist(LIST_DYN_OBJ_FORTES *list){

    LIST_DYN_OBJ_FORTES* actual_trace = (LIST_DYN_OBJ_FORTES*)malloc(sizeof(LIST_DYN_OBJ_FORTES));

    actual_trace->adresse_mem_map_FORTES = list->adresse_mem_map_FORTES;
    actual_trace->block_MEM_Point_FORTES = list->block_MEM_Point_FORTES;
    actual_trace->map_ID_func_FORTES     = list->map_ID_func_FORTES;
    actual_trace->map_is_DYNAM_FORTES    = list->map_is_DYNAM_FORTES;
    actual_trace->map_setFree_FORTES     = list->map_setFree_FORTES;
    actual_trace->map_isUnion_FORTES     = list->map_isUnion_FORTES;
    actual_trace->map_linePreCode_FORTES = list->map_linePreCode_FORTES;
    actual_trace->map_namevar_FORTES     = list->map_namevar_FORTES;
    actual_trace->map_typevar_FORTES     = list->map_typevar_FORTES;
    actual_trace->next_item_FORTES       = list_LOG_complete_TRACE;

    list_LOG_complete_TRACE = actual_trace;

    //Disable this, cuz generated large Traces, eg, > 1 GB

//    FILE *filelog;
//    /* open the file */
//    filelog = fopen("/tmp/tmp_map_currentlog.tmp", "a");
//    if (filelog == NULL) {
//     printf("I couldn't open log.map for writing.\n");
//     exit(0);
//    }
//
//    /* Strategy to short the trace log generated by the execution */
//    // TODO
//
//    /* write to the file */
//    fprintf(filelog, "%4d\t| %18p| %18p| %4d\t\t| %4d\t\t| %4d\t\t| %8d\t| %4d\t| %8s\t",
//                    count_all_list_FORTES,
//                    list->adresse_mem_map_FORTES,
//                    list->block_MEM_Point_FORTES,
//                    list->map_ID_func_FORTES,
//                    list->map_is_DYNAM_FORTES,
//                    list->map_setFree_FORTES,
//                    list->map_isUnion_FORTES,
//                    list->map_linePreCode_FORTES,
//                    list->map_namevar_FORTES);
//
//    //Write the value VAR
//    gb_actualfile = filelog;
//    if(list->block_MEM_Point_FORTES != NULL){
//        print_by_type(list->adresse_mem_map_FORTES, list->map_typevar_FORTES, 1, 1);
//    }else{
//        print_by_type(list->adresse_mem_map_FORTES, list->map_typevar_FORTES, 0, 1);
//    }
//    fprintf(filelog,"\n");
//
//
//    /* close the file */
//    fclose(filelog);

}




/*******************************************************************/
// Function to print the mapping status until a k deepth of the code
//DEBUG - TODO: BUG
/*******************************************************************/
void print_debug(LIST_DYN_OBJ_FORTES *list)
{
  printf("   #\t|      Address\t    | Address points to\t| Function ID\t|   IS DYNAMIC\t| Set FREE \t|  IS UNION \t| Line \n");
  printf("-------------------------------------------------------------------------------------------------------\n");
  LIST_DYN_OBJ_FORTES *aux;
  int save_count = count_all_list_FORTES;
  for (aux=list; aux!=NULL; aux=aux->next_item_FORTES){
	printf("%4d\t| ", count_all_list_FORTES);
    printf("%18p| ", aux->adresse_mem_map_FORTES);
    
    printf("%18p| ", aux->block_MEM_Point_FORTES);    
	
    printf("%4d\t\t| ", aux->map_ID_func_FORTES);
    printf("%4d\t\t|", aux->map_is_DYNAM_FORTES);
    printf("%4d\t\t|", aux->map_setFree_FORTES);
    printf("%4d\t\t|", aux->map_isUnion_FORTES);
    printf("%4d\n", aux->map_linePreCode_FORTES);
        
    count_all_list_FORTES--;
  }
  
  count_all_list_FORTES=save_count;
  printf("-------------------------------------------------------------------------------------------------------\n");
}
/*******************************************************************/



/*******************************************************************/
// Function to TRACE LOG of the program execution at current point
//DEBUG
/*******************************************************************/
void PRINT_TRACE_LOG()
{
    printf("Counter-example - Trace Log: \n");
    printf("   #\t|      Address\t    | Address points to\t| Function ID\t|  IS DYNAMIC\t|   Set FREE \t| IS UNION \t| Line \t| Var Name \t| Var Value\n");
    //printf("   #\t|      Address\t    | Address points to\t| Function ID\t|  IS DYNAMIC\t|   Set FREE \t| IS UNION \t| Line\n");

//    FILE *fp;
//    int ch;
//
//    fp = fopen("/tmp/tmp_map_currentlog.tmp","r"); // read mode
//
//    if( fp == NULL )
//    {
//      perror("Error while opening the file.\n");
//      exit(EXIT_FAILURE);
//    }
//
//    while( ( ch = fgetc(fp) ) != EOF )
//        printf("%c",ch);
//
//    fclose(fp);

  //printf("-------------------------------------------------------------------------------------------------------\n");
  LIST_DYN_OBJ_FORTES *aux;
  LIST_DYN_OBJ_FORTES *aux_2;
  LIST_DYN_OBJ_FORTES *aux_3;

  int save_count = count_all_list_FORTES;

  //list_LOG_complete_TRACE
  //for (aux=list_CE_trace; aux!=NULL; aux=aux->next_item_FORTES){
  for (aux=list_LOG_complete_TRACE; aux!=NULL; aux=aux->next_item_FORTES){

    int count_found = 0;
    for (aux_2=list_LOG_complete_TRACE; aux_2!=NULL; aux_2=aux_2->next_item_FORTES){

        if (aux->map_linePreCode_FORTES == aux_2->map_linePreCode_FORTES &&
            aux->map_namevar_FORTES == aux_2->map_namevar_FORTES){

            if(count_found > 0){
                //printf("Removing %4d == %4d \n", aux_2->map_linePreCode_FORTES, aux->map_linePreCode_FORTES);
                aux->next_item_FORTES = aux_2->next_item_FORTES;
                free(aux_2);
            }

            count_found += 1;


        }

    }






//
//	printf("%4d\t| ", count_all_list_FORTES);
//    printf("%18p| ", aux->adresse_mem_map_FORTES);
//
//    printf("%18p| ", aux->block_MEM_Point_FORTES);
//
//    //Getting the variables values
////    if(aux->block_MEM_Point_FORTES != NULL){
////        print_by_type(aux->block_MEM_Point_FORTES, aux->map_typevar_FORTES, 1,0);
////    }else{
////        print_by_type(aux->adresse_mem_map_FORTES, aux->map_typevar_FORTES, 0,0);
////    }
////
////    printf("\t|");
//
//
//
//    printf("%4d\t\t| ", aux->map_ID_func_FORTES);
//    printf("%4d \t\t|", aux->map_is_DYNAM_FORTES);
//    printf("%4d \t\t|", aux->map_setFree_FORTES);
//    printf("%4d \t\t|", aux->map_isUnion_FORTES);
//    printf("%4d \n", aux->map_linePreCode_FORTES);
//
//    count_all_list_FORTES--;

    }

//    for (aux_3=list_LOG_complete_TRACE; aux_3!=NULL; aux_3=aux_3->next_item_FORTES){
//        printf("%4d \n", aux_3->map_linePreCode_FORTES);
//    }



  reverse_trace_to_print(list_LOG_complete_TRACE);

  count_all_list_FORTES=save_count;
  //printf("-------------------------------------------------------------------------------------------------------\n");
}



reverse_trace_to_print(LIST_DYN_OBJ_FORTES* list){
    if(list==NULL)
        return;

    reverse_trace_to_print(list->next_item_FORTES);
    //printf("The node is %d\n", list->map_linePreCode_FORTES);
    //printf("-- %p\n", list->block_MEM_Point_FORTES);
    //printf("-- %d \n", list->map_ID_func_FORTES);

    printf("%4d\t| %18p| %18p| %4d\t\t| %4d\t\t| %4d\t\t| %8d\t| %4d\t| %8s\t|",
            count_all_list_FORTES,
            list->adresse_mem_map_FORTES,
            list->block_MEM_Point_FORTES,
            list->map_ID_func_FORTES,
            list->map_is_DYNAM_FORTES,
            list->map_setFree_FORTES,
            list->map_isUnion_FORTES,
            list->map_linePreCode_FORTES,
            list->map_namevar_FORTES);


    //Write the value VAR
    //printf("\n");
    //printf("The node is %s\n", list->map_typevar_FORTES);
    //printf("The node is %s\n", list->adresse_mem_map_FORTES);
    if(list->block_MEM_Point_FORTES == NULL){
        printf("NULL");
    }else{
        if(FLAG_to_IS_VALID_POINTER_FORTES == 0){
            print_by_type(list->adresse_mem_map_FORTES, list->map_typevar_FORTES, 1, 0);
        }else{
            printf("--");
        }
        //print_by_type(list->adresse_mem_map_FORTES, list->map_typevar_FORTES, 0, 0);
    }
    printf("\n");
    //printf("The node is %s\n", list->map_typevar_FORTES);


}



/*******************************************************************/


/*******************************************************************/
/**
 * Functions to mapilute the list to map the C code
 **/ 
/*******************************************************************/
 
/**
 * Function: add2List
 * GOAL: Function to add a new element into the list 
 **/ 
LIST_DYN_OBJ_FORTES* add2List(LIST_DYN_OBJ_FORTES* list, void *adress, void *block, char *namevar, int ID,
                              int status, int setFree, int isUnion, char *typevar, int linePreCode){
	
	LIST_DYN_OBJ_FORTES* aux_blk_cr;
    LIST_DYN_OBJ_FORTES* aux_tmp2;
	LIST_DYN_OBJ_FORTES* novo = (LIST_DYN_OBJ_FORTES*)malloc(sizeof(LIST_DYN_OBJ_FORTES));  	
	
	
    int get_IS_DYN = !(IS_DYNAMIC_OBJECT_FORTES(list, (void *)adress, (void *)block));
    
    //printf("Pre-condition to UPDATE: %d and %d \n",get_IS_DYN,setFree);
    
    // Aditional step when we have a adress that is not in list log field adress, but the block adress is already in list log
    // ex. for C library use and then free function
    if(get_IS_DYN == 1 && setFree == 1){
        if (FLAG_DEBUG == 1){
            printf("LINE FREE new log: %d \n", linePreCode);
        }
        //Just create a new element, i.e., mapping another object
        novo->map_is_DYNAM_FORTES = 0;
        novo->map_setFree_FORTES = setFree;
        novo->map_isUnion_FORTES = isUnion;
        novo->map_typevar_FORTES = typevar;
        novo->adresse_mem_map_FORTES = (void *)adress; //get the adress from value point out
        novo->block_MEM_Point_FORTES = (void *)block;
        novo->map_namevar_FORTES = namevar;
        novo->map_ID_func_FORTES = ID;			
        novo->map_linePreCode_FORTES = linePreCode;
        novo->next_item_FORTES = list;
        
        // For other points in the log list
        /**
         * Now updating block status in other positions from memory	from that block points out
         * i.e., when we have other pointer points out for this block address
         * that has been updated, e.g., A -> 0xfh1 and also P -> 0xfh1.
         * */				
        //Running in all list again
        for (aux_tmp2=list; aux_tmp2!=NULL; aux_tmp2=aux_tmp2->next_item_FORTES){				
                            
            if(novo->block_MEM_Point_FORTES == aux_tmp2->block_MEM_Point_FORTES && 
               aux_tmp2->map_is_DYNAM_FORTES != status){					
                    aux_tmp2->map_is_DYNAM_FORTES = 0;	
                    aux_tmp2->map_setFree_FORTES = setFree;	
            }					
        }
        
        
    }else{
        
        //When we have a Pointer = Dinamyc Block of the Memory
        //Review -  here we just check if the object points out to a
        // dynamic memory block
        //printf("Is not a UPDATE, checking block update \n");
        //printf("Re-checking is_DYN: %d \n",!(IS_DYNAMIC_OBJECT_FORTES(list, (void *)adress, (void *)block)));
        
        
        /**         
         * Neste considerar o status do malloc se não for um malloc ai então efetuar a busca na lista
         * */
        if(status == 0){
            int flag_stop = 0;
            for (aux_blk_cr=list; (aux_blk_cr!=NULL && flag_stop == 0); aux_blk_cr=aux_blk_cr->next_item_FORTES){            
                if (aux_blk_cr->block_MEM_Point_FORTES == (void *)block){                
                    //printf("%p <> %p in Line = %d\n",aux_blk_cr->block_MEM_Point_FORTES, (void *)block, aux_blk_cr->map_linePreCode_FORTES);       
                    status = aux_blk_cr->map_is_DYNAM_FORTES;
                    flag_stop = 1;                
                }            
            }
            
        }
            
	
        
        //Just create a new element, i.e., mapping another object
        novo->adresse_mem_map_FORTES = (void *)adress; //get the adress from value point out
        novo->block_MEM_Point_FORTES = (void *)block;
        novo->map_namevar_FORTES = namevar;
        novo->map_ID_func_FORTES = ID;		
        novo->map_is_DYNAM_FORTES = status;
        novo->map_setFree_FORTES = setFree;
        novo->map_isUnion_FORTES = isUnion;
        novo->map_typevar_FORTES = typevar;
        novo->map_linePreCode_FORTES = linePreCode;
        novo->next_item_FORTES = list;
    }
	
	
    if (FLAG_DEBUG == 1){
        printf("------------------ DEGUB -----------------------\n");	
        print_debug(novo);
    }

	list_CE_trace = novo;
	write_logfile_maplist(novo);
	return novo;
		
}





 
/**
 * Function: add2List
 * 
 * GOAL:Mapping the features  of the memory address.
 * 		First of all is searching the memory address from the object 
 * 		that will be mapped, if the memory address is not found 
 * 		then  this address is added to the list. However, if the address
 * 		is already in the map list we the follows rules:
 * 		- (1) The address features are updated only if is execued the command { FREE() }
 * 		- (2) For each { address assignment } we just add a new log from this in the map list 
 * 
 * [TO DO] 
 * 		=> Improve performace and check leak02.c
 * */ 
LIST_DYN_OBJ_FORTES* mark_map_FORTES(LIST_DYN_OBJ_FORTES* list, void *adress, void *block, char *namevar, int ID,
                                     int status, int setFree, int isUnion, char *typevar, int linePreCode)
{   
	LIST_DYN_OBJ_FORTES* aux;
    LIST_DYN_OBJ_FORTES* aux_tmp2;
 	
	int flag_indetified = 0;
	int flag_update = 0;		
	
	//Searching the memory address in the map list
	int flag_STOP_look = 0; //stop searching
    
	for (aux=list; aux!=NULL; aux=aux->next_item_FORTES){
		
		flag_indetified = 0;			
		
		//check if the adress is in the list from MAP		
		if (aux->adresse_mem_map_FORTES == (void *)adress){		

			flag_indetified = 1;			
			flag_STOP_look = 1; //We find the object			
			
			//idenify if is FREE command
			if(aux->map_setFree_FORTES != setFree && aux->block_MEM_Point_FORTES == (void *)block){
                
                if (FLAG_DEBUG == 1){
                    printf("\n IS a FREE for { %p } on { %p } in line: %d \n", aux->adresse_mem_map_FORTES, (void *)block, linePreCode);
                }
                
                //printf("TEST: %p \n", list->adresse_mem_map_FORTES);
				flag_update = 1;
                
                
                aux->map_is_DYNAM_FORTES = status; 
                //printf("Status: %d \n", aux->map_is_DYNAM_FORTES);
				aux->block_MEM_Point_FORTES = (void *)block; //get the adress from value point out				
				aux->map_setFree_FORTES = setFree;
                aux->map_isUnion_FORTES = isUnion;
                aux->map_typevar_FORTES = typevar;
                //printf("Free: %d \n", aux->map_setFree_FORTES);
				aux->map_linePreCode_FORTES = linePreCode;
                
                
                // For other points in the log list
                /**
				 * Now updating block status in other positions from memory	from that block points out
				 * i.e., when we have other pointer points out for this block address
				 * that has been updated, e.g., A -> 0xfh1 and also P -> 0xfh1.
				 * */				
				//Running in all list again
				for (aux_tmp2=list; aux_tmp2!=NULL; aux_tmp2=aux_tmp2->next_item_FORTES){				
									
					if(aux->block_MEM_Point_FORTES == aux_tmp2->block_MEM_Point_FORTES && 
					   aux_tmp2->map_is_DYNAM_FORTES != status){					
							aux_tmp2->map_is_DYNAM_FORTES = status;	
							aux_tmp2->map_setFree_FORTES = setFree;	
					}					
				}
                
                
                //print_debug(list);
                
                list_CE_trace = list;
                //Here is adopted aux to print cuz we want the actual address that has been updated
                write_logfile_maplist(aux);
                return list;
                
			}
				
		}
		
		
		if(flag_indetified == 1 && flag_update == 0){
			//add a new log to mapping
			count_all_list_FORTES++;			
            
            //printf("%d ADD NEW LOG -> %d : malloc status = %d\n", count_all_list_FORTES, linePreCode, status);
			
            if (FLAG_DEBUG == 1){
                printf("%d ADD NEW LOG \n", count_all_list_FORTES);
            }
			
            return add2List(list, (void *)adress, (void *)block, namevar, ID, status, setFree, isUnion, typevar, linePreCode);
		}					
	}
	
	
	
	/** 
	 * If the object was not find we added this new element
	 * */		
	if(flag_indetified == 0){
		count_all_list_FORTES++;
        
        //printf("%d ADD -> %d \n", count_all_list_FORTES, linePreCode);
		
        if (FLAG_DEBUG == 1){
            printf("%d ADD \n", count_all_list_FORTES);
        }
        
        //printf("Add new address: %p -> %p \n", (void *)adress, (void *)block);
		
        return add2List(list, (void *)adress, (void *)block, namevar, ID, status, setFree, isUnion, typevar, linePreCode);
	}else{		
		//print_debug(list);
		//return list;
		list_CE_trace = list;
		write_logfile_maplist(list);
        return list;
	} 
  
}
/*******************************************************************/


/*******************************************************************/
// Functions to check memory claims
/*******************************************************************/
/* Check memory leak [INCOPLETE]*/
int CHECK_MEMORY_LEAK(LIST_DYN_OBJ_FORTES* list, int ID_FUNC, int numLineCCode)
{
	LIST_DYN_OBJ_FORTES* aux;
	int cont_find = 0, cont_is = 0;
		
	for (aux=list; aux!=NULL; aux=aux->next_item_FORTES){				
		
		//1º - devemos verificar se os objeto na função todos foram liberados		
		//if(aux->map_ID_func_FORTES == ID_FUNC){										
		if(aux->map_is_DYNAM_FORTES == 1){

		    PRINT_TRACE_LOG();

			printf("\n");
            printf("[Failed] \n");
			printf(" VIOLATED PROPERTY: Memory Leak \n");			
			printf("\t Location at original code in line: %d \n", numLineCCode);
            printf("\t Last use at original code in line: %d \n", aux->map_linePreCode_FORTES);
			printf("\n"); 								
			return 0;  //dereference failure: forgotten memory 			
		}		
		//}
		
	}
	
	return 1;
	
}

////////// Check invalid_free
/**
 * Verificar se o objeto a ser liberado ainda é dinâmico 
 * */
int INVALID_FREE(LIST_DYN_OBJ_FORTES* list, void *block, int numLineCCode)
{
  
  if(!(IS_DYNAMIC_OBJECT_FORTES(list, (void *)0, (void *)block))){
    return 1; //TRUE
  }else{
    //free = INVALID FREE cuz object already free

    PRINT_TRACE_LOG();

    printf("\n");
    printf("[Failed] \n");
    printf(" VIOLATED PROPERTY: Invalid FREE\n");				
    printf("\t Location at original code in line: %d \n", numLineCCode);
    printf("\t Last use at original code in line: %d \n", line_of_the_last_use);
    printf("\n");
    return 0; //FALSE
  }
  
  //LIST_DYN_OBJ_FORTES* aux;
  //for (aux=list; aux!=NULL; aux=aux->next_item_FORTES){
	  
	  //if((aux->block_MEM_Point_FORTES == (void *)block)){            
            
			//if(aux->map_is_DYNAM_FORTES == 0){ //free = INVALID FREE cuz object already free
				////printf("%p -> 0 \n", (void *)block);
				//printf("\n");
                //printf("[Failed] \n");
				//printf(" VIOLATED PROPERTY: Invalid FREE\n");				
				//printf("\t Location at original code in line: %d \n", numLineCCode);
                //printf("\t Last use at original code in line: %d \n", aux->map_linePreCode_FORTES);
				//printf("\n");
				//return 0; //FALSE
			//}else{
				////printf("%p -> 1 \n", (void *)block);
				//return 1; //TRUE
			//}      
	  //} 
	     
   //}
}
 
 

/* Check se é um objeto dinâmico [**NEW**]*/
int IS_DYNAMIC_OBJECT_FORTES(LIST_DYN_OBJ_FORTES* list, void *adress, void *block)
{
  LIST_DYN_OBJ_FORTES* aux;
  int cont_find = 0;
  
  for (aux=list; aux!=NULL; aux=aux->next_item_FORTES){    
    
    if (aux->block_MEM_Point_FORTES == (void *)block && aux->map_is_DYNAM_FORTES == 0){
        line_of_the_last_use = aux->map_linePreCode_FORTES;
        return 1; //negation            
    }
    
    if (aux->block_MEM_Point_FORTES == (void *)block && aux->map_is_DYNAM_FORTES == 1){
        //printf("==================> IS_DYN: %p <> %p \n",aux->block_MEM_Point_FORTES,(void *)block);
        line_of_the_last_use = aux->map_linePreCode_FORTES;
        return 0; //negation            
    }
    
   }
   
   return 1; //genation is not DYN  
   
}

/* Get the last block address pointed by some address*/
void * GET_LAST_ADDR_POINTS_TO(LIST_DYN_OBJ_FORTES* list, void *adress){
    
    LIST_DYN_OBJ_FORTES* aux;
    
    for (aux=list; aux!=NULL; aux=aux->next_item_FORTES){        
        if(aux->adresse_mem_map_FORTES == (void *)adress){
            //printf("GET_LAST_ADDR_POINTS_TO %p \n", aux->block_MEM_Point_FORTES);
            line_of_the_last_use = aux->map_linePreCode_FORTES;
            return aux->block_MEM_Point_FORTES;
        }
    }
    
    return NULL;
    
}


/* Get the last block address pointed by some address*/
void * GET_LAST_ADDR_FROM_MALLOC(LIST_DYN_OBJ_FORTES* list, void *adress){
    
    LIST_DYN_OBJ_FORTES* aux;
    
    for (aux=list; aux!=NULL; aux=aux->next_item_FORTES){         
        if(aux->adresse_mem_map_FORTES == (void *)adress && aux->map_is_DYNAM_FORTES == 1){            
            line_of_the_last_use = aux->map_linePreCode_FORTES;
            return aux->block_MEM_Point_FORTES;
        }
    }
    
    return NULL;
    
}



/* Check if some address block is egual to other one*/
int CHECK_OVERWRITE_ADDR(void *actual_block, void *last_block, int numLineCCode){
    //TODO set a menssage here
    //printf("%p == %p", (void*)actual_block, (void*)last_block);
    if((void*)actual_block == (void*)last_block){
        return 1; //TRUE
    }else{

        PRINT_TRACE_LOG();

        printf("\n");
        printf("[Failed] \n");
        printf(" VIOLATED PROPERTY: Overwrite Memory Address\n");
        printf("\t Location at original code in line: %d \n", numLineCCode);
        printf("\t Last use at original code in line: %d \n", line_of_the_last_use);
        printf("\t Actual address   : %p \n", (void*)actual_block);
        printf("\t Original address : %p \n", (void*)last_block);
        printf("\n");
        return 0; //FALSE
    }
}



/* Check se o objeto esta na lista */
int IS_VALID_OBJECT_FORTES(LIST_DYN_OBJ_FORTES* list, void *adress, void *block)
{
  LIST_DYN_OBJ_FORTES* aux;
  for (aux=list; aux!=NULL; aux=aux->next_item_FORTES){
	  if((aux->block_MEM_Point_FORTES == (void *)block)){
		  if(aux->map_is_DYNAM_FORTES == 0){ //free = INVALID Block
			return 0; //FALSE is not a valid object
		}      
	  }    
   }
   
   for (aux=list; aux!=NULL; aux=aux->next_item_FORTES){
	   if (aux->adresse_mem_map_FORTES == (void *)adress){
			return 1; //TRUE
	   }else{
		    return 0; //FALSE is not a valid object
	   }
   }	
   
  return 0; //FALSE is not a valid object
}


/* Check if a pointer is valid */
/**
 * 
 * GOAL: Verifiy if the address or the address the a pointer points to 
 *       is valid, i.e., if has i the log list
 * 
 * WARNING: Test more cuz the claim trainslation [TODO]
 * 
 * */
int IS_VALID_POINTER_FORTES(LIST_DYN_OBJ_FORTES* list, void *adress, void *block)
{
  LIST_DYN_OBJ_FORTES* aux, *aux2;
  //Search by block address
  for (aux=list; aux!=NULL; aux=aux->next_item_FORTES){
      //printf("%p -- %p \n", aux->block_MEM_Point_FORTES, (void *)block);
	  if((aux->block_MEM_Point_FORTES == (void *)block)){
            //We have two possible valids conclusion
            //(1) is the block is a dynamic value
            if(aux->map_is_DYNAM_FORTES == 0){
                return 1; //is not a dynamic value, so invalid address - 1 cuz the negation -- BUG
            }
            //printf("aux->map_is_DYNAM_FORTES == 0 \n");

            //(2) is address used by another valid variable
            for (aux2=list; aux2!=NULL; aux2=aux2->next_item_FORTES){
                if (aux->adresse_mem_map_FORTES == (void *)block){
                    //printf("%p -- %p \n", aux->adresse_mem_map_FORTES, (void *)block);
                    return 0; //  Is a valid address - 0 cuz the negation -- NO BUG
                }
            }

            // The two two possible of valids pointer was not okay
			return 1; //FALSE is not a valid object - 0 cuz the negation
	  }    
   }
   
   //Search by current address
   for (aux=list; aux!=NULL; aux=aux->next_item_FORTES){
	   if (aux->adresse_mem_map_FORTES == (void *)adress){
			return 0; //TRUE - 0 cuz the negation
	   }
   }

  FLAG_to_IS_VALID_POINTER_FORTES = 1;
  return 1; //FALSE is not a valid pointer - 1 cuz the negation
}


/* Identify error location in the program  */
int __VERIFIER_error(int numLineCCode){

    PRINT_TRACE_LOG();

    printf("\n");
    printf("[Failed] \n");
    printf(" VIOLATED PROPERTY: __VERIFIER_error \n");
    printf("\t Location at original code in line: %d \n", numLineCCode);
    printf("\n");
    assert(0);
    //exit(EXIT_FAILURE);

}

/*******************************************************************/

/*******************************************************************/
// UTILS
// ----------------------------------------------------------------
// Utility function to find ceiling of r in arr[l..h]
int findCeil(int arr[], int r, int l, int h)
{
    int mid;
    while (l < h)
    {
         mid = l + ((h - l) >> 1);  // Same as mid = (l+h)/2
        (r > arr[mid]) ? (l = mid + 1) : (h = mid);
    }
    return (arr[l] >= r) ? l : -1;
}
 
// The main function that returns a random number from arr[] according to
// distribution array defined by freq[]. n is size of arrays.
int myRand(int arr[], int freq[], int n)
{
    // Create and fill prefix array
    int prefix[n], i;
    prefix[0] = freq[0];
    for (i = 1; i < n; ++i)
        prefix[i] = prefix[i - 1] + freq[i];
 
    // prefix[n-1] is sum of all frequencies. Generate a random number
    // with value from 1 to this sum
    int r = (rand() % prefix[n - 1]) + 1;
 
    // Find index of ceiling of r in prefix arrat
    int indexc = findCeil(prefix, r, 0, n - 1);
    return arr[indexc];
}

//int __VERIFIER_nondet_int(void)
int __VERIFIER_nondet_int(void){
    //int arr[]  = {0, 1};
    //int upperlimit = 10;
    int arr[]  = {0, 5};
    int freq[] = {30, 65};
    int i, n = sizeof(arr) / sizeof(arr[0]); 
    // Use a different seed value for every run.
    srand(time(NULL));
    
    return (myRand(arr, freq, n));
}
// ----------------------------------------------------------------
/*******************************************************************/

