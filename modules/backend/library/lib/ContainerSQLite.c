#include <stdio.h>
#include <stdlib.h>
#include "../header/Container.h"
#include "sqlite3.h"

extern int getpid();
#define SQLITE_FILE_NAME_SIZE 50
#define SQLITE_SQL_COMMAND_SIZE 1000

static Bool execute_no_callback(sqlite3* db, const char* sql) {
  char* zErrMsg = 0;
  int rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    return FALSE;
  } else {
    return TRUE;
  }
}

const char* create_table_heaplog_sql =
    "CREATE TABLE `HeapLog` ( `id` INTEGER NOT NULL UNIQUE, `line` INTEGER "
    "NOT NULL, `scope` INTEGER NOT NULL, `value` INTEGER NOT NULL, `size` "
    "INTEGER NOT NULL, `primitiveSize` INTEGER NOT NULL, `functionName` TEXT "
    "NOT NULL )";

static Bool insert_table_heaplog(sqlite3* db, MEMORY_HEAP_ROW* row,
                                 unsigned id) {
  char sql[SQLITE_SQL_COMMAND_SIZE];
  const char* base =
      "INSERT INTO `HeapLog` (`id`,`line`,`scope`,`value`,`size`, "
      "`primitiveSize`, `functionName`) VALUES ( %u, %u, %u, %ld, %d, %d, %s "
      ");";
  snprintf(sql, SQLITE_SQL_COMMAND_SIZE, base, id, row->line, row->scope,
           (long)row->value, row->size, row->size_of_primitive,
           row->function_name);
  return execute_no_callback(db, sql);
}

const char* create_table_listlog_sql =
    "CREATE TABLE `ListLog` "
    " ( `id` INTEGER NOT NULL UNIQUE, `varName` TEXT NOT NULL, "
    "`memoryAddress` INTEGER NOT NULL, `memoryAddressPointsTo` INTEGER NOT "
    "NULL, `scope` TEXT NOT NULL, `isDynamic` INTEGER NOT NULL, `isFree` "
    "INTEGER NOT NULL, `lineNumber` INTEGER NOT NULL, `functionName` TEXT "
    "NOT NULL, `stepOnExecution` INTEGER NOT NULL, PRIMARY KEY(`id`) )";

static Bool insert_table_listlog(sqlite3* db, LIST_LOG_ROW* row, unsigned id) {
  char sql[SQLITE_SQL_COMMAND_SIZE];
  const char* base =
      "INSERT INTO `ListLog` "
      "(`id`,`varName`,`memoryAddress`,`memoryAddressPointsTo`, "
      "`scope`, `isDynamic`, isFree`, `lineNumber`, `functionName`, "
      "`stepOnExecution` ) VALUES ( %u, %s, %ld, %ld, %d, %d, %d, %d, %s, %u  "
      ");";
  snprintf(sql, SQLITE_SQL_COMMAND_SIZE, base, id, row->var_name,
           row->memory_address, row->memory_address_points_to, row->scope,
           row->is_dynamic, row->is_free, row->line_number, row->function_name,
           row->step_on_execution);
  return execute_no_callback(db, sql);
}

const char* create_table_memoryallocationlog_sql =
    "CREATE TABLE `MemoryAllocationLog` ( `id` INTEGER NOT NULL UNIQUE, "
    "`addr` INTEGER NOT NULL, `isFree` INTEGER NOT NULL, `size` INTEGER NOT "
    "NULL, PRIMARY KEY(`id`) )";

static Bool insert_table_memoryallocationlog(sqlite3* db,
                                             MEMORY_ALLOCATIONS_ROW* row,
                                             unsigned id) {
  char sql[SQLITE_SQL_COMMAND_SIZE];
  const char* base =
      "INSERT INTO `MemoryAllocationLog` "
      "(`id`,`addr`,`isFree`, "
      "`size`) VALUES ( %u, %ld, %d, %u "
      ");";
  snprintf(sql, SQLITE_SQL_COMMAND_SIZE, base, id, row->addr, row->is_free,
           row->size);
  return execute_no_callback(db, sql);
}

const char* create_table_nondetlog_sql =
    "CREATE TABLE `NonDetLog` ( `id` INTEGER NOT NULL UNIQUE, `type` INTEGER "
    "NOT NULL, `line` INTEGER NOT NULL, `stepOnExecution` INTEGER NOT NULL, "
    "`scope` INTEGER NOT NULL, `value` INTEGER NOT NULL, `functionName` TEXT "
    "NOT NULL, PRIMARY KEY(`id`) )";

static Bool insert_table_nondetlog(sqlite3* db, NONDET_CALL* row, unsigned id) {
  char sql[SQLITE_SQL_COMMAND_SIZE];
  const char* base =
      "INSERT INTO `NonDetLog` "
      "(`id`,`type`,`line`, `stepOnExecution`, "
      "`scope`, `value`, `functionName` ) VALUES ( %u, %d, %u, %u, %u, %ld, %s "
      ");";
  snprintf(sql, SQLITE_SQL_COMMAND_SIZE, base, id, row->type, row->line,
           row->step_on_execution, row->scope, row->value, row->function_name);
  return execute_no_callback(db, sql);
}

const char* create_table_trackbblog_sql =
    "CREATE TABLE `TrackBBLog` ( `id` INTEGER NOT NULL UNIQUE, `line` "
    "INTEGER NOT NULL, `functionName` TEXT NOT NULL, PRIMARY KEY(`id`) )";

static Bool insert_table_trackbblog(sqlite3* db, TRACK_BB_ROW* row,
                                    unsigned id) {
  char sql[SQLITE_SQL_COMMAND_SIZE];
  const char* base =
      "INSERT INTO `TrackBBLog` "
      "(`id`,`line`, `functionName`) "
      " VALUES ( %u, %u, %s "
      ");";
  snprintf(sql, SQLITE_SQL_COMMAND_SIZE, base, id, row->line,
           row->function_name);
  return execute_no_callback(db, sql);
}

static void create_tables(sqlite3* db) {
  char* zErrMsg = 0;
  sqlite3_exec(db, create_table_heaplog_sql, NULL, 0, &zErrMsg);
  sqlite3_exec(db, create_table_listlog_sql, NULL, 0, &zErrMsg);
  sqlite3_exec(db, create_table_memoryallocationlog_sql, NULL, 0, &zErrMsg);
  sqlite3_exec(db, create_table_nondetlog_sql, NULL, 0, &zErrMsg);
  sqlite3_exec(db, create_table_trackbblog_sql, NULL, 0, &zErrMsg);
}

MAP2CHECK_CONTAINER new_container(enum Container_Type type) {
  MAP2CHECK_CONTAINER container;
  container.size = 0;
  int proccess_id = getpid();
  char* file_suffix = "container.map2check.db";
  sqlite3* db;
  char name[SQLITE_FILE_NAME_SIZE];
  snprintf(name, SQLITE_FILE_NAME_SIZE, "%d-%s", proccess_id, file_suffix);
  sqlite3_open(name, &db);

  create_tables(db);
  container.values = db;
  container.type = type;
  return container;
}

Bool free_container(MAP2CHECK_CONTAINER* container) {
  int proccess_id = getpid();
  char* file_suffix = "container.map2check.db";
  char name[SQLITE_FILE_NAME_SIZE];
  snprintf(name, SQLITE_FILE_NAME_SIZE, "rm -rf %d-%s", proccess_id,
           file_suffix);
  system(name);
  sqlite3* db = (sqlite3*)container->values;
  sqlite3_close(db);
  return TRUE;
}

Bool append_element(MAP2CHECK_CONTAINER* container, void* row) {
  Bool result;
  sqlite3* db = (sqlite3*)container->values;
  switch (container->type) {
    case LIST_LOG_CONTAINER:
      result = insert_table_listlog(db, (LIST_LOG_ROW*)row, container->size);
      break;
    case ALLOCATION_LOG_CONTAINER:
      result = insert_table_memoryallocationlog(
          db, (MEMORY_ALLOCATIONS_ROW*)row, container->size);
      break;
    case NONDET_LOG_CONTAINER:
      result = insert_table_nondetlog(db, (NONDET_CALL*)row, container->size);
      break;
    case HEAP_LOG_CONTAINER:
      result = insert_table_heaplog(db, (MEMORY_HEAP_ROW*)row, container->size);
      break;
    case TRACKBB_LOG_CONTAINER:
      result = insert_table_trackbblog(db, (TRACK_BB_ROW*)row, container->size);
      break;
    default:
      result = FALSE;
  }
  container->size++;
  return result;
}

void* get_element_at(unsigned index, MAP2CHECK_CONTAINER container) {
  return NULL;
}

void free_used_element(void* addr) { free(addr); }