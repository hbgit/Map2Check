#include "util.h"
#include <stdio.h>
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
  system("rm -rf data.map2check.bin");
  printf("TEST 1_1\n");
  int n = 20;
  B_TREE_ROW row[n];
  B_TREE bt = B_TREE_CREATE(HEAP_LOG_CONTAINER);
  int i = 0;
  for(;i<n;i++) {
    row[i].index = i;
    B_TREE_INSERT(&bt, &row[i]);
  }
  DumpTree(&bt);
  B_TREE_FREE(&bt);
}

// SECOND PART
void TEST_2_1() {
  system("rm -rf data.map2check.bin");
  printf("TEST 2_1\n");
  int n = 4;
  B_TREE_ROW row[n];
  B_TREE bt = B_TREE_CREATE(HEAP_LOG_CONTAINER);
  int i = 0;
  for(;i<n;i++) {
    row[i].index = i + 1;
    B_TREE_INSERT(&bt, &row[i]);
  }
  DumpTree(&bt);

  B_TREE_PAGE temp;
  DISK_READ(&bt, bt.root->stream_pos,&temp);
  DISK_READ(&bt, bt.root->children[0]->stream_pos, &temp);
  DISK_READ(&bt, bt.root->children[1]->stream_pos, &temp);
  
  B_TREE_FREE(&bt);
}

// An abnomous number of page should release and work normally
void TEST_2_2() {
  system("rm -rf data.map2check.bin");
  printf("TEST 2_2\n");
  int n = 10000;
  B_TREE_ROW row[n];
  B_TREE bt = B_TREE_CREATE(HEAP_LOG_CONTAINER);
  int i = 0;
  for(;i<n;i++) {
    row[i].index = i + 1;
    B_TREE_INSERT(&bt, &row[i]);
  }

  B_TREE_ROW* result;
  for(i=1;i<(n+1);i++) {
    result = B_TREE_SEARCH(&bt, i);
    if(result == NULL) {
      printf("Something wrong at id: %d\n", i);
      exit(-1);
    }
    if(result->index != i) {
      printf("GOT: %d\t EXPECTED: %d\n", result->index, i);      
    } else {
      printf("OK\n");
    }
  }
  
  B_TREE_FREE(&bt);
}
// THIRD PART

// DEALLOCATION OF TREE


// MAIN FUNCTION

int main(int argc, char *argv[argc]) {
  if(argc > 1) {
    int suit = atoi(argv[1]);
    switch (suit) {
      case 0: {
        TEST_1_1();
            break;
      }
      case 1: {
        TEST_2_1();
        TEST_2_2();
        break;            
      }        
    }
  } else {
    TEST_1_1();
    TEST_2_1();
  
  } 
  return 0;
}
