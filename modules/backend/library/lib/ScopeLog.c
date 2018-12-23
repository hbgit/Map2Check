#include "../header/ScopeLog.h"
#include <stdio.h>
#include <string.h>

const char* scope_log_file = "scope_log.csv";

static SCOPE_ROW* search_element_with_id(MAP2CHECK_CONTAINER* container,
                                         unsigned id) {
  unsigned i;

  for (i = 0; i < container->size; i++) {
    SCOPE_ROW* iRow = get_element_at(i, *container);
    if (iRow->id == id) return iRow;
  }
  return NULL;
}

Bool mark_scope_log(MAP2CHECK_CONTAINER* container, SCOPE_ROW row) {
  if (container->type != HEAP_LOG_CONTAINER) {
    return FALSE;
  }
  SCOPE_ROW* old = search_element_with_id(container, row.id);
  if (old != NULL) {
    old->parent = row.parent;
    return TRUE;
  }
  return append_element(container, &row);
}

Bool is_scope_valid(MAP2CHECK_CONTAINER* container, SCOPE_ROW row,
                    SCOPE_ROW toCheck) {
  SCOPE_ROW helper = row;
  while (1) {
    // Check if is same scope ID
    if (helper.id == toCheck.id) {
      // Check if is same function scope
      return !strcmp(helper.function_name, toCheck.function_name);
    }
    // Exit condition
    if (helper.id == helper.parent) return FALSE;
    // Helper will receive his parent
    helper = *(search_element_with_id(container, helper.parent));
  }
}

SCOPE_ROW new_scope_row(unsigned id, unsigned parent,
                        const char* function_name) {
  SCOPE_ROW row;
  strncpy(row.function_name, function_name, FUNCTION_MAX_LENGTH_NAME);
  row.id = id;
  row.parent = parent;
  return row;
}