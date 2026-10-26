/**
 * Copyright (C) 2014 - 2020 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include <stdlib.h>
#include "../header/WitnessGeneration.h"
#include "../header/AnalysisMode.h"
#include "../header/NonDetGenerator.h"
#include "../header/TrackBBLog.h"

void generate_aux_files(MAP2CHECK_CONTAINER *trackbb_log, Bool violation) {
  trackbb_log_to_file(trackbb_log);
  analysis_generate_aux_witness_files();
  /* Only a violating execution has an input vector worth recording, and under
   * KLEE every forked state reaches this point. They all write the same file,
   * so without the guard a state that took a different branch clobbers the
   * violating one's log -- which is exactly what produced a test case with the
   * right first input and a wrong second one. */
  /* The seed exchange replays the fuzzer's NON-violating queue entries to
   * learn their typed vectors, so it asks for the log explicitly. Only that
   * replay sets the variable; KLEE's forked states never do. */
  if (violation || getenv("MAP2CHECK_SEED_REPLAY") != NULL) {
    nondet_generate_aux_witness_files();
  }
}

#include <stdio.h>
void generate_confirmation_file() {
  FILE *output = fopen("map2check_checked_error", "w");
  fprintf(output, "END OF TIME\n");
  fclose(output);
}
