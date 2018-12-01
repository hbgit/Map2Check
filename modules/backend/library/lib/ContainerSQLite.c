#include "../header/Container.h"
#include "./SQLiteConsts.h"
#include <sqlite3.h>

MAP2CHECK_CONTAINER new_container(enum Container_Type type) {
  MAP2CHECK_CONTAINER container;
  container.size = 0;
  int proccess_id = getpid();
  char* file_suffix;
  sqlite3 *db;
  switch (container.type) {
    case LIST_LOG_CONTAINER:
      file_suffix = "listlog.map2check.bin";
      break;
    case ALLOCATION_LOG_CONTAINER:
      file_suffix = "allocationlog.map2check.bin";
      break;
    case NONDET_LOG_CONTAINER:
      file_suffix = "nondetlog.map2check.bin";
      break;
    case HEAP_LOG_CONTAINER:
      file_suffix = "heaplog.map2check.bin";
      break;
    case TRACKBB_LOG_CONTAINER:
      file_suffix = "trackbblog.map2check.bin";
      break;
    default:
      file_suffix = "default.map2check.bin";
      break;
  }
  char name[B_TREE_FILE_NAME_SIZE];
  snprintf(name, B_TREE_FILE_NAME_SIZE, "%d-%s", proccess_id, file_suffix);
  int rc = sqlite3_open(name, &db);
  container.values = db;
  container.type = type;  
  return container;
}

Bool free_container(MAP2CHECK_CONTAINER* container) {
  int proccess_id = getpid();
  char* file_suffix;
  switch (container->type) {
    case LIST_LOG_CONTAINER:
      file_suffix = "listlog.map2check.bin";
      break;
    case ALLOCATION_LOG_CONTAINER:
      file_suffix = "allocationlog.map2check.bin";
      break;
    case NONDET_LOG_CONTAINER:
      file_suffix = "nondetlog.map2check.bin";
      break;
    case HEAP_LOG_CONTAINER:
      file_suffix = "heaplog.map2check.bin";
      break;
    case TRACKBB_LOG_CONTAINER:
      file_suffix = "trackbblog.map2check.bin";
      break;
    default:
      file_suffix = "default.map2check.bin";
      break;
  }
  char name[B_TREE_FILE_NAME_SIZE];
  snprintf(name, B_TREE_FILE_NAME_SIZE, "rm -rf %d-%s", proccess_id,
           file_suffix);
  //system(name);
  sqlite3 *db = (sqlite3*) container->values;
  sqlite3_close(db);
  return TRUE;
}