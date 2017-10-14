#include "TrackBBLog.h"
#include <stdio.h>

const char* trackbb_log_file = "track_bb_log.st";

TRACK_BB_ROW trackbb_new_row(unsigned line, const char* function_name) {
  
  TRACK_BB_ROW row;
  
  row.function_name = function_name;
  row.line = line;
  
  return row;
  
}

void trackbb_log_to_file(MAP2CHECK_CONTAINER* list) {
  FILE* output = fopen(trackbb_log_file, "w");  
  int i = 0;
  for(;i< list->size; i++) {
    TRACK_BB_ROW* row = (TRACK_BB_ROW*) get_element_at(i, *list);
    fprintf(output, "%u;", row->line);
    fprintf(output, "%s\n", row->function_name);    
  }
  fclose(output);
}
