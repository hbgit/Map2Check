#include "BTree.h"

#include <stdio.h>
#include <stdlib.h>

B_TREE_PAGE* DISK_READ(B_TREE* btree, long int stream_pos) {
  return NULL;
}

Bool DISK_WRITE(B_TREE* btree, B_TREE_PAGE* object) {
  return FALSE;
}

B_TREE_ROW* B_TREE_SEARCH(B_TREE* btree, unsigned key) {
  return NULL;
}

B_TREE B_TREE_CREATE(enum Container_Type type) {
  B_TREE bt;
  bt.type = type;
  return bt;
}

Bool B_TREE_INSERT(B_TREE* btree, B_TREE_ROW* row) {
  return FALSE;
}

Bool B_TREE_INSERT_NONFULL(B_TREE* btree, B_TREE_ROW* row, B_TREE_PAGE* page) {
  return FALSE;
}

Bool B_TREE_SPLIT_CHILD(B_TREE* btree, B_TREE_PAGE* parent,
                        int index, B_TREE_PAGE* child) {
  return FALSE;
}
