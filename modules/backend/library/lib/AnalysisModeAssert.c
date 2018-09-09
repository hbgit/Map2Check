#include "../header/AnalysisMode.h"
#include "../header/Map2CheckFunctions.h"
#include "../header/PropertyGenerator.h"
/**
 * @brief Asserts that condition is true
 * @param condition Integer to check, if 0 false else true
 * @param line      Line of the assert
 * @param func_name Function name
 */
void map2check_assert(int condition, int line, const char* func_name) {
    if(!condition) {
        write_property(ASSERT, line, func_name);
        map2check_error();
    }
}

void __VERIFIER_assert(int condition) {
    // DO NOTHING
}

void analysis_init() {
  // DO NOTHING
}

void analysis_destroy() {
  // DO NOTHING
}

Bool analysis_is_program_correct() { return TRUE; }

void analysis_generate_aux_witness_files() {
  // DO NOTHING
}
