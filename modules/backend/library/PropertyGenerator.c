#include "PropertyGenerator.h"
#include <stdio.h>
#include <klee/klee.h>
// #include <stdlib.h>
// #include <string.h>

const char* violated_property_file = "map2check_property";
const char* violated_property_file_unknown = "map2check_property_klee_unknown";
const char* violated_property_file_memtrack = "map2check_property_klee_memtrack";
const char* violated_property_file_deref = "map2check_property_klee_deref";
const char* violated_property_file_free = "map2check_property_klee_free";
const char* violated_property_file_overflow = "map2check_property_overflow";


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

void write_property_deref(int line, char *func) {
    FILE* output = fopen(violated_property_file_deref, "w");
    fprintf(output, "FALSE-DEREF\n");
    fprintf(output, "Line: %d\n", line);
    fprintf(output, "Function: %s\n", func);
    fclose(output);
}

//void write_property_free() {
//    fprintf(output, "FALSE-FREE\n");
//    fprintf(output, "Line: %d\n", line);
//    fprintf(output, "Function: %s\n", function_name);
//}


void write_property(enum ViolatedProperty violated, int line, const char* function_name) {
    FILE* output = fopen("map2check_property", "w");

    switch(violated) {
        case NONE:
            fprintf(output, "NONE\n");
            break;
        case UNKNOWN:
            fprintf(output, "UNKNOWN\n");
            break;
        case FALSE_FREE:
        write_property_free(line,function_name);
            fprintf(output, "FALSE-FREE\n");
            fprintf(output, "Line: %d\n", line);
            fprintf(output, "Function: %s\n", function_name);
            break;
        case FALSE_DEREF:
            write_property_deref(line,function_name);
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

    }

    fclose(output);
}

void write_property_free(int line, const char *function_name)
{
    FILE* output = fopen(violated_property_file_free, "w");
    fprintf(output, "FALSE-FREE\n");
    fprintf(output, "Line: %d\n", line);
    fprintf(output, "Function: %s\n", function_name);
    fclose(output);
}
