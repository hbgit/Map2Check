/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include "Container.h"
#include "Map2CheckTypes.h"

#ifndef TrackBBLog_H
#define TrackBBLog_H

TRACK_BB_ROW trackbb_new_row(unsigned line, const char *function_name);

Bool is_in_tracked(unsigned line, MAP2CHECK_CONTAINER *log);

/**
 * Write elements from TRACKBB_LOG to a FILE
 * @param row Pointer to the TRACKBB_LOG to be wrote
 */
void trackbb_log_to_file(MAP2CHECK_CONTAINER *list);

#endif
