#include "util.h"
#include "../BTree.h"

/* This test suit will check for error in BTree Implementation
 * it will be divided in some parts
 * (1) Data consistency in Primary Memory: Data should be
 *     added without errors, and the tree must balance itself correctly
 *
 * (2) Data consistency in Secondary Memory: Data should be
 *     read/write without errors
 *
 * (3) (Allocated Resources on RAM): All resources allocated
 *     should be deallocated
 *
 * (4) (Allocated Resources on Disk): All files from disk should be removed */

// FIRST PART

// TESTING INSERTION AND SEARCHING
void TEST_1_1() {
  B_TREE_ROW row1;
  B_TREE_ROW row2;
  B_TREE_ROW row3;

  B_TREE bt = B_TREE_CREATE(HEAP_LOG_CONTAINER);
  assert(B_TREE_INSERT(&bt, &row1), __LINE__);
  assert(B_TREE_INSERT(&bt, &row2), __LINE__);
  assert(B_TREE_INSERT(&bt, &row3), __LINE__);
  /*
  assert(B_TREE_SEARCH(&bt, 0) != NULL);
  assert(B_TREE_SEARCH(&bt, 1) != NULL);
  assert(B_TREE_SEARCH(&bt, 2) != NULL);

  assert(B_TREE_SEARCH(&bt, 10) == NULL);
  */
}


// MAIN FUNCTION
int main() {
  TEST_1_1();
  return 0;
}
