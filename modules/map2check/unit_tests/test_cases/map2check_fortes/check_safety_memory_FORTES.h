#ifndef CHECK_SAFETY_MEM_H_INCLUDED
#define CHECK_SAFETY_MEM_H_INCLUDED

typedef struct obj {
  void  *adresse_mem_map_FORTES;
  void  *block_MEM_Point_FORTES;
  int   map_ID_func_FORTES;
  int   map_is_DYNAM_FORTES; 
  
  struct obj *next_item_FORTES; 
} LIST_DYN_OBJ_FORTES;


LIST_DYN_OBJ_FORTES* mark_map_FORTES(LIST_DYN_OBJ_FORTES* list, void *adress, void *block,int ID, int status, int setFree, int linePreCode);

int CHECK_MEMORY_LEAK(LIST_DYN_OBJ_FORTES* list, int ID_FUNC);

int IS_DYNAMIC_OBJECT_FORTES(LIST_DYN_OBJ_FORTES* list, void *adress, void *block);

int IS_VALID_OBJECT_FORTES(LIST_DYN_OBJ_FORTES* list, void *adress, void *block);

int INVALID_FREE(LIST_DYN_OBJ_FORTES* list, void *block, int linePreCode);

void print_debug(LIST_DYN_OBJ_FORTES *list);

LIST_DYN_OBJ_FORTES *list_DIN_OBJT_FORTES=NULL; /* Para o mapeamento */	
#endif
