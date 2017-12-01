#ifndef BTREE_H_
#define BTREE_H_

#include "Map2CheckTypes.h"
#include "Container.h"

typedef long long int lll_int;

/* This file describes an API for a BTree
 *
 * The implementation is based on the book
 * T. H. Cormen, C. E. Leiserson, R. L. Rivest, C. Stein. Introduction to
 * Algorithms. The MIT Press. 2002, Chapter 18, but since Map2Check containers
 * don't need remotion, it will not be implemented. */

// TODO(rafa.sa.xp@gmail.com) Play a little bit to find the best size for tree properties

/* Each ROW uses about 64 bytes (assuming 32bit architecture),
 * since Cormen tells that usually disk page is 2^11 to 2^14 bytes,
 * then we could estimate disk page to 16KiB and order 100 */

#define B_TREE_ROW_SIZE 64U
#define B_TREE_MAP2CHECK_ORDER 2U

/* Since Some programs can use 4GiB of RAM in seconds, we should
 * unload some pages before that occurs */
#define B_TREE_MAX_OPEN_PAGES 256U

/* We should have some kind of limit for infinite pograms or some programs will
 * end up using all secondary space, ~20GiB should be enought for now, considering
 * PAGE_SIZE ~ 136 bytes, then 20GB can hold 157903209 pages, so max height should be 3 */
#define B_TREE_MAX_HEIGHT 3U

/** Struct that holds values on B_TREE */
typedef struct B_TREE_ROW {
  /** Current index of element */
  unsigned index;
  /** Row Content*/
  CONTAINER_ROW value;  
} B_TREE_ROW;

/** Struct that holds pages of B_TREE */
typedef struct B_TREE_PAGE {
  // TODO(rafa.sa.xp@gmail.com) should remove stream_pos since root note already have
  /** Number of keys curently stored in node */
  unsigned n;
  /* An array of size 2t - 1 containing values*/
  B_TREE_ROW* rows[B_TREE_MAP2CHECK_ORDER*2 - 1];
  /** Current index on FILE, this is redudant information, but
   *  make implementation easier */
  lll_int stream_pos;  
  /** Returns true if node is a leaf */
  Bool isLeaf;
  /** An arrray of size 2t containing pointers to children */    
  struct B_TREE_PAGE* children[B_TREE_MAP2CHECK_ORDER*2];
  /** An arrray of size 2t containing pointers to children in FILE*/    
  lll_int references[B_TREE_MAP2CHECK_ORDER*2];
} B_TREE_PAGE;

/** Struct that manipulates the B-TREE */
typedef struct B_TREE {  
  /** Root of tree */
  B_TREE_PAGE* root;
  /** Current loaded pages */
  unsigned currentLoadedPages;
  /** Annotation to get Container Type */  
  enum Container_Type type;
} B_TREE;

/* DISK_READ and DISK_WRITE will be somewhat different
 * from what Cormen proposed, in our implementation
 * DISK_READ will only be executed if there is need to
 * load the file, adding the stream_pos to the argument.
 * Since the B_TREE_PAGE contains the the children struct
 * it is easier to check if the page was loaded or not in
 * another helper function */

/** Read page from disk
 *  @param btree Pointer to B_TREE
 *  @param stream_pos Position on the FILE where page is
 *  @return Pointer to B_TREE_PAGE or NULL */
B_TREE_PAGE* DISK_READ(B_TREE* btree, long int stream_pos);

/** Write page to disk, if new page adds stream_pos, if not, then updates
 *  @param btree Pointer to B_TREE
 *  @param object Pointer to page
 *  @return Success of operation */
Bool DISK_WRITE(B_TREE* btree, B_TREE_PAGE* object);

/** Search though B_TREE pages to get ROW
 *  @param btree Pointer to B_TREE
 *  @param key Key to be found
 *  @return Pointer to ROW or NULL */
B_TREE_ROW* B_TREE_SEARCH(B_TREE* btree, unsigned key);

B_TREE B_TREE_CREATE(enum Container_Type type);
Bool B_TREE_INSERT(B_TREE* btree, B_TREE_ROW* row);
Bool B_TREE_INSERT_NONFULL(B_TREE* btree, B_TREE_ROW* row, B_TREE_PAGE* page);
Bool B_TREE_SPLIT_CHILD(B_TREE* btree, B_TREE_PAGE* parent,
                        int index, B_TREE_PAGE* child);

B_TREE_PAGE* B_TREE_PAGE_CREATE(B_TREE* btree);
void B_TREE_FREE(B_TREE* btree);

// TODO(rafa.sa.xp@gmail.com) After Implementation and test, remove this
void DumpTree(B_TREE* btree);
void DumpTreePage( B_TREE_PAGE* page);
void DumpTreePageChildren( B_TREE_PAGE* page);
void DumpTreeHelper(unsigned index, B_TREE_PAGE* page);
#endif
