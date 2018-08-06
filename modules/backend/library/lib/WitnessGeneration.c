#include "../header/WitnessGeneration.h"
#include "../header/AnalysisMode.h"
#include "../header/NonDetGenerator.h"
#include "../header/TrackBBLog.h"

void generate_aux_files(MAP2CHECK_CONTAINER *trackbb_log) {
  trackbb_log_to_file(trackbb_log);
  analysis_generate_aux_witness_files();
  nondet_generate_aux_witness_files();
}
