
#include "../header/Map2CheckFunctions.h"
#include "../header/AnalysisMode.h"
#include "../header/Container.h"
#include "../header/NonDetGenerator.h"
#include "../header/PropertyGenerator.h"
#include "../header/TrackBBLog.h"
#include "../header/WitnessGeneration.h"
#include <stdio.h>
#include <stdlib.h>

unsigned Map2CheckCurrentStep;

extern void __assert_fail(const char *assertion, const char *file,
                          unsigned int line, const char *function);

MAP2CHECK_CONTAINER trackbb_log;

Bool sv_comp = FALSE;
Bool gotError = FALSE;

void map2check_init(int isSvComp) {
  Map2CheckCurrentStep = 0;

  trackbb_log = new_container(TRACKBB_LOG_CONTAINER);

  if (isSvComp) {
    sv_comp = TRUE;
  }

  analysis_init();
  nondet_init();
}

unsigned map2check_get_current_step() { return Map2CheckCurrentStep; }
void map2check_next_current_step() { Map2CheckCurrentStep++; }

void map2check_assume(int expr) {
  if (!expr) {
    /* printf("Assume Failed\n"); */
    map2check_destroy();
    nondet_cancel();
  } else {
    /* printf("Assume ok\n"); */
  }
}

void map2check_track_bb(unsigned line, const char *function_name) {
  if (!(is_in_tracked(line, &trackbb_log))) {
    TRACK_BB_ROW *row = malloc(sizeof(TRACK_BB_ROW));
    *row = trackbb_new_row(line, function_name);
    append_element(&trackbb_log, row);
  }
}

void __VERIFIER_error() {
  // DO NOTHING
}

void map2check_target_function(const char *func_name, int scope, int line) {
  write_property(TARGET_REACHED, line, func_name);
  map2check_error();
}

void map2check_success() {
  // write_property(NONE, 0, "");
  map2check_exit();
}

void map2check_error() {
  gotError = TRUE;
  map2check_exit();
}

void map2check_destroy() {
  static Bool alreadyReleased = FALSE;
  if (alreadyReleased)
    return;
  alreadyReleased = TRUE;
  free_container(&trackbb_log);
  analysis_destroy();
  nondet_destroy();
}

void map2check_exit() {
  if (gotError == FALSE) {
    gotError = !analysis_is_program_correct();
  }

  generate_aux_files(&trackbb_log);
  /* gotError = TRUE; */
  if (gotError == TRUE) {
    abort();
  }

  map2check_destroy();
}
