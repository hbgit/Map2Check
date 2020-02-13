/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include "../header/WitnessGeneration.h"
#include "../header/AnalysisMode.h"
#include "../header/NonDetGenerator.h"
#include "../header/TrackBBLog.h"

void generate_aux_files(MAP2CHECK_CONTAINER *trackbb_log) {
  trackbb_log_to_file(trackbb_log);
  analysis_generate_aux_witness_files();
  nondet_generate_aux_witness_files();
}

#include <stdio.h>
void generate_confirmation_file() {
  FILE *output = fopen("map2check_checked_error", "w");
  fprintf(output, "END OF TIME\n");
  fclose(output);
}
