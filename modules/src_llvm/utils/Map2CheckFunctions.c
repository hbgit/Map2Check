#include "Map2CheckFunctions.h"
#include <klee/klee.h>

int Map2CheckCurrentStep;

MAP2CHECK_CONTAINER list_log;
MAP2CHECK_CONTAINER klee_log;
MAP2CHECK_CONTAINER allocation_log;


void map2check_init() {
    Map2CheckCurrentStep = 0;
    list_log = new_container(LIST_LOG_CONTAINER);
    klee_log = new_container(KLEE_LOG_CONTAINER);
    allocation_log = new_container(ALLOCATION_LOG_CONTAINER);
    write_property(UNKNOWN, 0, "");
}

void map2check_klee_int(unsigned line, unsigned scope, int value, const char* function_name) {  
  int* result = (int*) malloc(sizeof(int));
  *result = value;
  KLEE_CALL kleeCall = new_klee_call(INTEGER, line, scope, result,function_name, Map2CheckCurrentStep);
  Map2CheckCurrentStep++;
  printf("adding klee call\n");
  append_element(&klee_log, &kleeCall);

}


void map2check_add_store_pointer(void* var, void* value, unsigned scope, const char* name, int line, const char* function_name) {
  
  enum MemoryAddressStatus status = check_address_allocation_log(&allocation_log, (long) value);
  Bool isDynamic;
  Bool isFree;

  // Since we check in AllocationLog we can only known if the address is STATIC, FREE or DYNAMIC 
  switch (status) {
  case STATIC:
    isDynamic = FALSE;
    isFree = FALSE;
    break;
  case FREE:
    isDynamic = FALSE;
    isFree = TRUE;
    break;
  case DYNAMIC:
    isDynamic = TRUE;
    isFree = FALSE;
    break;
   default:
     isDynamic = FALSE;
     isFree = FALSE; 
  }
  LIST_LOG_ROW row = new_list_row((long) var,(long) value, scope, isDynamic, isFree, line, name, function_name, Map2CheckCurrentStep);
  Map2CheckCurrentStep++;
//   long oldAddress = getOldReference(name, &list_log);
  printf("Adding element list log\n")  ;
  append_element(&list_log, &row);
  update_reference_list_log((long)value, status, line);
}

int map2check_non_det_int() {
  int non_det;
  klee_make_symbolic(&non_det,
		     sizeof(non_det),
		     "non_det_int");
  return non_det;
}

void map2check_target_function(const char* func_name, int scope, int line) {
    write_property(TARGET_REACHED, line, func_name);
    map2check_exit();
}

void map2check_free_resolved_address(void* ptr, unsigned line, const char* function_name) {
    Bool error = is_invalid_free((long) ptr, &list_log);
     if(error) {
        write_property(FALSE_FREE, line, function_name);
        map2check_exit();
     }  
    MEMORY_ALLOCATIONS_ROW row =  new_memory_row((long) ptr, TRUE); 
    printf("Adding element allocation log\n")  ;
    append_element(&allocation_log, &row);
    
    update_reference_list_log((long) ptr, FREE, line);
}

void map2check_malloc(void* ptr, int size) {
    MEMORY_ALLOCATIONS_ROW row =  new_memory_row((long) ptr, FALSE); 
    row.size = size;
    printf("Adding element allocation log\n")  ;
    append_element(&allocation_log, &row);
}

void map2check_free(const char* name, void* ptr, unsigned scope, unsigned line,  const char* function_name) {
    
    long* addr = (long*) ptr;

    LIST_LOG_ROW listRow = new_list_row((long) ptr, (long) *addr, scope, FALSE,
				  TRUE, line, name, function_name, Map2CheckCurrentStep);


    Map2CheckCurrentStep++;

    Bool error = is_invalid_free((long) *addr, &list_log);  
    printf("Adding element list log\n")  ;  
    append_element(&list_log, &listRow);

    if(error) {        
        write_property(FALSE_FREE, line, function_name);
        map2check_exit();
     }  
     
    MEMORY_ALLOCATIONS_ROW row =  new_memory_row((long) *addr, TRUE); 
    printf("Adding element list log\n")  ;
    append_element(&allocation_log, &row);

    update_reference_list_log((long) *addr, FREE, line);  
}

void map2check_success() {
    if(!valid_allocation_log(&allocation_log)) {
        write_property(FALSE_MEMTRACK, 0, "");
        map2check_exit();
    } 
    else {
       write_property(NONE, 0, "");
    }

    klee_log_to_file(klee_log);
    free_container(&klee_log);

    list_log_to_file(&list_log);
    free_container(&list_log);
    free_container(&allocation_log);
    
}

void map2check_exit() {
    klee_log_to_file(klee_log);
    free_container(&klee_log);

    list_log_to_file(&list_log);
    free_container(&list_log);
    free_container(&allocation_log);

    klee_assert(0);
}

void update_reference_list_log(long address, enum MemoryAddressStatus status, unsigned line) {

}