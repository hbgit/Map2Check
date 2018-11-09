#include "../header/PropertyGenerator.h"
#include <stdio.h>
// #include <klee/klee.h>
// #include <stdlib.h>
// #include <string.h>

const char* violated_property_file = "map2check_property";
const char* violated_property_file_unknown = "map2check_property__unknown";
const char* violated_property_file_memtrack = "map2check_property_memtrack";
const char* violated_property_file_deref = "map2check_property_deref";
const char* violated_property_file_free = "map2check_property_free";
const char* violated_property_file_overflow = "map2check_property_overflow";
const char* violated_property_file_assert = "map2check_property_assert";

void write_property_unknown() {
  FILE* output = fopen(violated_property_file_unknown, "w");
  fprintf(output, "UNKNOWN\n");
  fclose(output);
}

void write_property_memtrack() {
  FILE* output = fopen(violated_property_file_memtrack, "w");
  fprintf(output, "FALSE-MEMTRACK\n");
  fclose(output);
}

void write_property_deref(int line, const char* func) {
  FILE* output = fopen(violated_property_file_deref, "w");
  fprintf(output, "FALSE-DEREF\n");
  fprintf(output, "Line: %d\n", line);
  fprintf(output, "Function: %s\n", func);
  fclose(output);
}

void write_property(enum ViolatedProperty violated, int line,
                    const char* function_name) {
  FILE* output = fopen("map2check_property", "w");

  switch (violated) {
    case NONE:
      fprintf(output, "NONE\n");
      break;
    case UNKNOWN:
      fprintf(output, "UNKNOWN\n");
      break;
    case FALSE_FREE:
      write_property_free(line, function_name);
      fprintf(output, "FALSE-FREE\n");
      fprintf(output, "Line: %d\n", line);
      fprintf(output, "Function: %s\n", function_name);
      break;
    case FALSE_DEREF:
      write_property_deref(line, function_name);
      fprintf(output, "FALSE-DEREF\n");
      fprintf(output, "Line: %d\n", line);
      fprintf(output, "Function: %s\n", function_name);
      break;
    case FALSE_MEMTRACK:
      write_property_memtrack();
      fprintf(output, "FALSE-MEMTRACK\n");
      break;
    case TARGET_REACHED:
      fprintf(output, "TARGET-REACHED\n");
      fprintf(output, "Line: %d\n", line);
      fprintf(output, "Function: %s\n", function_name);
      break;
    case FALSE_OVERFLOW:
      write_property_overflow(line, function_name);
      fprintf(output, "OVERFLOW\n");
      fprintf(output, "Line: %d\n", line);
      fprintf(output, "Function: %s\n", function_name);
      break;
    case ASSERT:
      write_property_assert(line, function_name);
      fprintf(output, "ASSERT\n");
      fprintf(output, "Line: %d\n", line);
      fprintf(output, "Function: %s\n", function_name);
      break;
  }

  fclose(output);
}

void write_property_free(int line, const char* function_name) {
  FILE* output = fopen(violated_property_file_free, "w");
  fprintf(output, "FALSE-FREE\n");
  fprintf(output, "Line: %d\n", line);
  fprintf(output, "Function: %s\n", function_name);
  fclose(output);
}

void write_property_overflow(int line, const char* function_name) {
  FILE* output = fopen(violated_property_file_overflow, "w");
  fprintf(output, "OVERFLOW\n");
  fprintf(output, "Line: %d\n", line);
  fprintf(output, "Function: %s\n", function_name);
  fclose(output);
}

void write_property_assert(int line, const char* function_name) {
  FILE* output = fopen(violated_property_file_assert, "w");
  fprintf(output, "ASSERT\n");
  fprintf(output, "Line: %d\n", line);
  fprintf(output, "Function: %s\n", function_name);
  fclose(output);
}
