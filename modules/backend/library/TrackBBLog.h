#include "Map2CheckTypes.h"
#include "Container.h"

#ifndef TRACKBBLOG_H
#define TRACKBBLOG_H

TRACK_BB_ROW trackbb_new_row(unsigned line, const char* function_name);

Bool is_in_tracked(unsigned line, MAP2CHECK_CONTAINER* log);

/**
 * Write elements from TRACKBB_LOG to a FILE
 * @param row Pointer to the TRACKBB_LOG to be writed
 */
void trackbb_log_to_file(MAP2CHECK_CONTAINER* list);



#endif
