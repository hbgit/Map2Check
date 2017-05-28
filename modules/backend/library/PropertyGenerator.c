#include "PropertyGenerator.h"
#include <stdio.h>
#include <klee/klee.h>
// #include <stdlib.h>
// #include <string.h>

const char* violated_property_file = "map2check_property";
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
            fprintf(output, "FALSE-FREE\n");
            fprintf(output, "Line: %d\n", line);
            fprintf(output, "Function: %s\n", function_name);
            break;
        case FALSE_DEREF:
            fprintf(output, "FALSE-DEREF\n");
            fprintf(output, "Line: %d\n", line);
            fprintf(output, "Function: %s\n", function_name);
            break;
        case FALSE_MEMTRACK:
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
