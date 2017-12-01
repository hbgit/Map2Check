#include "BTree.h"

#include <stdio.h>
#include <stdlib.h>

// TODO(rafa.sa.xp@gmail.com) Add SEMAPHORE for file manipulation

B_TREE_PAGE* DISK_READ(B_TREE* btree, long int stream_pos) {
  return NULL;
}

Bool DISK_WRITE(B_TREE* btree, B_TREE_PAGE* object) {
  return TRUE;
}

static B_TREE_ROW* B_TREE_SEARCH_HELPER(B_TREE* btree,
                                        B_TREE_PAGE* page, unsigned key) {
  int i = 0;
  while((i < page->n) && (key > page->rows[i]->index)) {
    i++;
  }
  if ((i < page->n) && (key == page->rows[i]->index)) {
    return page->rows[i];
  }

  if(page->isLeaf) {
    return NULL;
  } else {
    if(page->children[i] == NULL) {
      page->children[i] = DISK_READ(btree, page->references[i]);
      if(page->children[i] == NULL) {
        return NULL;
      }
    }
  }

  return B_TREE_SEARCH_HELPER(btree, page->children[i], key);
}

B_TREE_ROW* B_TREE_SEARCH(B_TREE* btree, unsigned key) {
  return B_TREE_SEARCH_HELPER(btree, btree->root, key);
}

B_TREE B_TREE_CREATE(enum Container_Type type) {
  B_TREE bt;
  bt.type = type;
  bt.root = B_TREE_PAGE_CREATE(&bt);
  bt.root->isLeaf = TRUE;
  return bt;
}

Bool B_TREE_INSERT(B_TREE* btree, B_TREE_ROW* row) {
  B_TREE_PAGE* r = btree->root;
  if(r->n == (2*B_TREE_MAP2CHECK_ORDER - 1)) {
    B_TREE_PAGE* s = B_TREE_PAGE_CREATE(btree);
    if(s == NULL) {
      return FALSE;
    }

    btree->root = s;
    s->isLeaf = FALSE;
    s->n = 0;
    s->references[1] = r->stream_pos;
    s->children[0] = r;

    if(!B_TREE_SPLIT_CHILD(btree, s, 0, r) ||
       !B_TREE_INSERT_NONFULL(btree, row, s)) {
      return FALSE;
    }

  } else {
    if(!B_TREE_INSERT_NONFULL(btree, row, r)) {
      return FALSE;
    }
  }
  return TRUE;
}

Bool B_TREE_INSERT_NONFULL(B_TREE* btree, B_TREE_ROW* row, B_TREE_PAGE* page) {
  if((row == NULL) || (page == NULL)) {    
    return FALSE;
  }
  int i = ((int)page->n) - 1;
  unsigned k = row->index;
  if(page->isLeaf) {
    while((i >= 0) && ( k < page->rows[i]->index)) {
      page->rows[i+1] = page->rows[i];
      i--;
    }
    page->rows[i+1] = row;
    page->n = page->n + 1;
    return DISK_WRITE(btree, page);
    
  } else {
    while((i >= 0) && ( k < page->rows[i]->index)) {
      i--;
    }

    i += 1;
    if(page->children[i] == NULL) {
      page->children[i] = DISK_READ(btree, page->references[i]);
      if(page->children[i] == NULL) {
         printf("WTFF");
        return FALSE;
      }
    }

    if(page->children[i]->n == (2*B_TREE_MAP2CHECK_ORDER - 1)) {
      if(!B_TREE_SPLIT_CHILD(btree, page, i, page->children[i])) {
        return FALSE;
      }
      if(k > page->rows[i]->index) {
        i += 1;        
      }      
    }
    return B_TREE_INSERT_NONFULL(btree, row, page->children[i]);
  }
}


/*      
 *  X  [...N,W,...]       ==>        X  [..., N,S,W,...]
 *          |             ==>                 /   \
 *  Y  [P,Q,R,S,T,U,V]    ==>          Y[P,Q,R]  Z[T,U,V]
 */  


Bool B_TREE_SPLIT_CHILD(B_TREE* btree, B_TREE_PAGE* X,
                        int index, B_TREE_PAGE* Y) {
  B_TREE_PAGE* Z = B_TREE_PAGE_CREATE(btree);
  if(Z == NULL) {
    return FALSE;
  }
  Z->isLeaf = Y->isLeaf;
  Z->n = B_TREE_MAP2CHECK_ORDER - 1;

  int j;
  for (j = 0; j < (B_TREE_MAP2CHECK_ORDER - 1); ++j) {
    Z->rows[j] = Y->rows[j + B_TREE_MAP2CHECK_ORDER];    
  }

  if(!Z->isLeaf) {
    for (j = 0; j < B_TREE_MAP2CHECK_ORDER; ++j) {
      Z->references[j] = Y->references[j + B_TREE_MAP2CHECK_ORDER];      
      Z->children[j] = Y->children[j + B_TREE_MAP2CHECK_ORDER];
      //Y->references[j + B_TREE_MAP2CHECK_ORDER] = -1;
      //Y->children[j + B_TREE_MAP2CHECK_ORDER] = NULL;    
    }    
  }

  Y->n = B_TREE_MAP2CHECK_ORDER - 1;
  for (j = X->n + 1; j > index + 1; --j) {
      X->references[j+1] = X->references[j];
      X->children[j+1] = X->children[j];
  }
  
  X->references[index+1] = Z->stream_pos;
  X->children[index+1] = Z;

 
  for(j = X->n + 1; j > index; j--) {
    X->rows[j+1] = X->rows[j];
  }

    
  /*  X->references[index] = Y->stream_pos;
      X->children[index] = Y;*/

  X->rows[index] = Y->rows[B_TREE_MAP2CHECK_ORDER - 1];
  X->n += 1;

  if(!DISK_WRITE(btree, Y) ||
     !DISK_WRITE(btree, Z) ||
     !DISK_WRITE(btree, X)) {
    return FALSE;
  }

  return TRUE;
}

B_TREE_PAGE* B_TREE_PAGE_CREATE(B_TREE* btree) {
  B_TREE_PAGE* btp = malloc(sizeof(B_TREE_PAGE));
  if(btp == NULL) {
    return NULL;
  }
  btp->n = 0;
  btp->stream_pos = 0;
  btp->isLeaf = TRUE;

  int i = 0;
  for(;i < B_TREE_MAP2CHECK_ORDER*2 - 1; i++) {
    btp->rows[i] = NULL;
    btp->children[i] = NULL;
    btp->references[i] = -1;
  }
  btp->references[i] = -1;
  if(!DISK_WRITE(btree, btp)) {
    return NULL;
  }
  return btp;
}

static void  B_TREE_FREE_ROW_HELPER(B_TREE_ROW* page) {

}

static void  B_TREE_FREE_PAGE_HELPER(B_TREE_PAGE* page) {
  if(page == NULL) {
    return;
  }
  int i;
  for (i = 0; i < page->n; i++) {
    B_TREE_FREE_ROW_HELPER(page->rows[i]);                           
  }

  if(!page->isLeaf) {
    for (i = 0; i <= page->n; i++) {
      B_TREE_FREE_PAGE_HELPER(page->children[i]);
    }
  }

  free(page);  
}

void B_TREE_FREE(B_TREE* btree) {  
  B_TREE_FREE_PAGE_HELPER(btree->root);
}


// TODO(rafa.sa.xp@gmail.com) After Implementation and test, remove this
void DumpTree(B_TREE* btree) {
  DumpTreeHelper(0, btree->root);  
}

void DumpTreePage( B_TREE_PAGE* page) {
  printf("Address: %p\n", page);
  printf("VALUES[ ");
  int i = 0;
  for (i = 0; i < page->n; ++i) {
    printf("%u ", page->rows[i]->index);
  }
  printf("]\n");    
}

void DumpTreePageChildren( B_TREE_PAGE* page) {
  printf("Children[ ");
  int i = 0;
  for (i = 0; i <= page->n; ++i) {
    printf("%p ", page->children[i]);
  }
  printf("]\n\n");
  
}

void DumpTreeHelper(unsigned index, B_TREE_PAGE* page) {
  if(page == NULL) {
    return;
  }
  
  printf("Current index: %u\tVALUES[ ",index);
  int i = 0;
  for (i = 0; i < page->n; ++i) {
    printf("%u ", page->rows[i]->index);
  }
  printf("]\n");
  if(!page->isLeaf) {
    for (i = 0; i <= page->n; ++i) {
      DumpTreeHelper(index + 1, page->children[i]);
    }
  }
  
}


