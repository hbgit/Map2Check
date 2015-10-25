#!/bin/bash

# ------------- Map2Check wrapper script to SVCOMP'15
# Usage: ./map2check_wrapper_script.sh <file.i> <witnessfile.graphml>

# Path to the Map2Check tool
#path_to_map2check=/home/herbert/Projects/Map2Check/map2check.py
path_to_map2check=/home/hrocha/ON_DEV/benchexec/Map2Check/map2check.py


# Command line, common to all tests.
# Run Map2Check 2 times. Each program in the category is executed 2 times,
# because of the nondeterministic model in the programs. It is important
# to note that from these 2 executions, we always consider the execution classified as
# FAILED (if any), i.e., an execution that the tool has identified a property violation.
map2check_options="--complete-check 2 --graphml-output --witnesspath"


# Memsafety cmdline property options
error_label = 0


while getopts "c:mh" arg; do
    case $arg in
        h)
            echo "Usage: $0 [options] path_to_benchmark
Options:
-h             Print this message
-c propfile    Specifythe given property file"
            ;;
        c)
            # Given the lack of variation in the property file... we don't
            # actually interpret it. Instead we have the same options to all
            # tests, except for the HeapReach, where we define the
            # error labels to be ERROR.
            if grep -q -E "LTL(G ! call(__VERIFIER_error()))" $OPTARG; then			
            	error_label = 1
            fi
            ;;
    esac
done

# Store the path to the file we're going to be checking.
benchmark=$1
# Store the path to the file to write the witness if the sourcefile
# contains a bug and the tool returns False (Error found)
witnesspath=$2

if test "${benchmark}" = ""; then
    echo "No benchmark given" >&2
    exit 1
fi

# The complete command to be executed
run_cmdline="${path_to_map2check} ${map2check_options} \"${witnesspath}\" \"${benchmark}\";"

# Invoke our command, wrapped in a timeout so that we can
# postprocess the results. `timeout` is part of coreutils on debian and fedora.
result_check=`timeout 895 bash -c "$run_cmdline"`

# Postprocessing: first, collect some facts
failed=`echo ${result_check} | grep -c "VERIFICATION FAILED"`
success=`echo ${result_check} | grep -c "VERIFICATION SUCCESSFUL"`

# Decide which result we determined here.
if [ $failed -gt 0 ]; then
    # Error path found
    # Gathering the property (i.e. valid-free, valid-memtrack,valid-deref) from result
    property=`echo ${result_check} | grep -o "FALSE.*"`
    # Print the property
    echo ${property}
#elif [ $success -gt 0 ]; then
#    echo "TRUE"
else
    echo "UNKNOWN"
    # Clean up after ourselves. This because, how the Map2Check was interruped than
    # it is possible to have some left over temporary file.
    # Get path from benchmark and then removing the temporary files
    srcbench="$(dirname ${benchmark})"

    scripttmp="${srcbench}/build_2_check.sh"
    headerlib="${srcbench}/check_safety_memory_FORTES.h"
    maplib="${srcbench}/check_safety_memory_FORTES.o"
    programtmp=`echo ${benchmark} | sed -e s/'.i'/'__mcf_new.c'/`
    execs="${srcbench}/*._mcf2check"


    if [ -f ${scripttmp} ]; then
        rm ${scripttmp}
    fi

    if [ -f ${headerlib} ]; then
        rm ${headerlib}
    fi

    if [ -f ${maplib} ]; then
        rm ${maplib}
    fi

    if [ -f ${programtmp} ]; then
        rm ${programtmp}
    fi

    count=`ls -1 ${execs} 2>/dev/null | wc -l`
    if [ $count != 0 ]; then
        rm ${execs}
    fi
fi
