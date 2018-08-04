#include "../header/Map2CheckFunctions.h"
#include "../header/AnalysisMode.h"
#include "../header/Container.h"
#include "../header/NonDetLog.h"
#include "../header/PropertyGenerator.h"
#include "../header/TrackBBLog.h"
#include "../header/WitnessGeneration.h"
#include <stdio.h>
#include <stdlib.h>

unsigned Map2CheckCurrentStep;

extern void __assert_fail(const char *assertion, const char *file,
                          unsigned int line, const char *function);

MAP2CHECK_CONTAINER klee_log;

MAP2CHECK_CONTAINER trackbb_log;

Bool sv_comp = FALSE;
Bool gotError = FALSE;

void map2check_init(int isSvComp) {
  Map2CheckCurrentStep = 0;
  klee_log = new_container(NONDET_LOG_CONTAINER);
  trackbb_log = new_container(TRACKBB_LOG_CONTAINER);

  if (isSvComp) {
    sv_comp = TRUE;
  }

  analysis_init();
}

unsigned map2check_get_current_step() { return Map2CheckCurrentStep; }
void map2check_next_current_step() { Map2CheckCurrentStep++; }

void map2check_track_bb(unsigned line, const char *function_name) {
  if (!(is_in_tracked(line, &trackbb_log))) {
    TRACK_BB_ROW *row = malloc(sizeof(TRACK_BB_ROW));
    *row = trackbb_new_row(line, function_name);
    append_element(&trackbb_log, row);
  }
}

void map2check_nondet_int(unsigned line, unsigned scope, int value,
                          const char *function_name) {
  /* int* result = (int*) malloc(sizeof(int)); */
  /* *result = value; */
  NONDET_CALL kleeCall = new_nondet_call(INTEGER, line, scope, value,
                                         function_name, Map2CheckCurrentStep);
  Map2CheckCurrentStep++;

  NONDET_CALL *row = malloc(sizeof(NONDET_CALL));
  *row = kleeCall;

  append_element(&klee_log, row);
  /* nondet_log_to_file(klee_log); */
}

void map2check_nondet_unsigned(unsigned line, unsigned scope, unsigned value,
                               const char *function_name) {
  NONDET_CALL kleeCall = new_nondet_call(UNSIGNED, line, scope, value,
                                         function_name, Map2CheckCurrentStep);
  Map2CheckCurrentStep++;

  NONDET_CALL *row = malloc(sizeof(NONDET_CALL));
  *row = kleeCall;

  append_element(&klee_log, row);
  /* nondet_log_to_file(klee_log); */
}

void map2check_nondet_char(unsigned line, unsigned scope, int value,
                           const char *function_name) {
  /* char* result = (char*) malloc(sizeof(char)); */
  /* *result = (char) value; */
  NONDET_CALL kleeCall = new_nondet_call(CHAR, line, scope, value,
                                         function_name, Map2CheckCurrentStep);
  Map2CheckCurrentStep++;

  NONDET_CALL *row = malloc(sizeof(NONDET_CALL));
  *row = kleeCall;

  append_element(&klee_log, row);
  /* nondet_log_to_file(klee_log); */
}

void map2check_nondet_pointer(unsigned line, unsigned scope, int value,
                              const char *function_name) {
  /* char* result = (char*) malloc(sizeof(char)); */
  /* *result = (char) value; */
  NONDET_CALL kleeCall = new_nondet_call(POINTER, line, scope, value,
                                         function_name, Map2CheckCurrentStep);
  Map2CheckCurrentStep++;

  NONDET_CALL *row = malloc(sizeof(NONDET_CALL));
  *row = kleeCall;

  append_element(&klee_log, row);
  /* nondet_log_to_file(klee_log); */
}

void map2check_nondet_ushort(unsigned line, unsigned scope, int value,
                             const char *function_name) {
  /* char* result = (char*) malloc(sizeof(char)); */
  /* *result = (char) value; */
  NONDET_CALL kleeCall = new_nondet_call(USHORT, line, scope, value,
                                         function_name, Map2CheckCurrentStep);
  Map2CheckCurrentStep++;

  NONDET_CALL *row = malloc(sizeof(NONDET_CALL));
  *row = kleeCall;

  append_element(&klee_log, row);
  /* nondet_log_to_file(klee_log); */
}

void map2check_nondet_long(unsigned line, unsigned scope, int value,
                           const char *function_name) {
  /* char* result = (char*) malloc(sizeof(char)); */
  /* *result = (char) value; */
  NONDET_CALL kleeCall = new_nondet_call(LONG, line, scope, value,
                                         function_name, Map2CheckCurrentStep);
  Map2CheckCurrentStep++;

  NONDET_CALL *row = malloc(sizeof(NONDET_CALL));
  *row = kleeCall;

  append_element(&klee_log, row);
  /* nondet_log_to_file(klee_log); */
}

void __VERIFIER_error() {
  // TODO: Check this function
  /*
  if (!valid_allocation_log(&allocation_log)) {
    write_property(FALSE_MEMTRACK, 0, "");
  } else {
    write_property(NONE, 0, "");
  }
  */
  map2check_exit();
  exit(0);
}

void map2check_target_function(const char *func_name, int scope, int line) {
  write_property(TARGET_REACHED, line, func_name);
  map2check_error();
}

void map2check_success() {
  write_property(NONE, 0, "");
  map2check_exit();
}

void map2check_error() {
  gotError = TRUE;
  map2check_exit();
}

void map2check_exit() {
  if (gotError == FALSE) {
    gotError = !analysis_is_program_correct();
  }
  generate_aux_files(&klee_log, &trackbb_log);

  free_container(&klee_log);
  free_container(&trackbb_log);
  analysis_destroy();

  /* gotError = TRUE; */
  if (gotError == TRUE) {
    abort();
  }
}
