#include "KleeLog.h"

#include <stdio.h>

const char* klee_log_file = "klee_log.csv";

Bool klee_log_to_file(MAP2CHECK_CONTAINER klee_container) {
  if(klee_container.type != KLEE_LOG_CONTAINER) {
      //printf("ERROR on klee_log_to_file called function with wrong container type\n");
      return FALSE;
  }
  
  FILE* output = fopen(klee_log_file, "w");

  int i = 0;
  unsigned size = klee_container.size;
  for(;i< size; i++) {
    KLEE_CALL* call = (KLEE_CALL*) get_element_at(i, klee_container); 
    if(call == NULL) {
        //printf("ERROR on klee_log_to_file get_element\n");
        return FALSE;
    }
    fprintf(output,"%d;", i);
    fprintf(output,"%d;", call->line);
    fprintf(output,"%d;", call->scope);
    fprintf(output,"%s;", call->function_name);
    fprintf(output,"%d;", call->step_on_execution);

    switch (call->type) {
      case INTEGER:
        fprintf(output,"%d\n", ((int)call->value));
        break;
      case CHAR:
        fprintf(output,"%c\n", ((char)call->value));
        break;
    }
  }
  fclose(output);
  return TRUE;
}

KLEE_CALL new_klee_call(enum NONDET_TYPE type, unsigned line, unsigned scope, long value, const char* function_name, unsigned step) {
  KLEE_CALL result;
  result.type = type;
  result.function_name = function_name;
  result.line = line;
  result.value = value;
  result.scope = scope;
  result.step_on_execution = step;  
  return result;
}
