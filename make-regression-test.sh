#!/bin/bash
# Running regression testing

# Checking regression type
# t = Travis service
# m = Map2Check tests
# s = SV-COMP tests

travis_flag=0
test_ce_format=0

# Checking if testing is adopting travis service
if [ -z "$1" ]; then
    echo "You should define the testing type:"
    echo "  t for travis    - small set of regression testing"
    echo "  m for map2check - set of regression testing"
    echo "  s for svcomp    - set of svcomp regression testing"
    exit 1
fi

if [ "$1" = "t" ]; then
    xml_doc_benchexec_name="map2check_regression_test_travis.xml"
    travis_flag=1
fi

if [ "$1" = "m" ]; then
    xml_doc_benchexec_name="map2check_regression_test.xml"
    travis_flag=0
fi

if [ "$1" = "s" ]; then
    if [ -z "$2" ]; then
        echo "You should define the xml definition path"
        echo "e.g.: tests/regression_test/xml_doc_benchexec/svcomp_by_category/"
        exit 1
    fi
    xml_doc_benchexec_name="$2"
    travis_flag=0
    # checking if you have the svcomp benchmark
    # the path should be inside the map2check directory
    if [ ! -e "sv-benchmarks" ]; then
	    echo "Downloading sv-benchmarks ..."
	    git clone https://github.com/sosy-lab/sv-benchmarks.git 
    fi
fi

if [ -z $1 ]; then
    echo "Please, define the regression type"
    exit 1
fi


echo "Adopting: $xml_doc_benchexec_name"

# Check if docker image is already build
# 1 - Build docker image available at https://github.com/hbgit/benchexecrun
has_docker_img=`docker images | grep -c "herberthb/benchexecrun"`
if [ $has_docker_img -gt 0 ]; then
  echo "Found benchexec image to run tests ..."
  is_main_dir=`ls | grep -c "make-regression-test.sh"`

  if [ $is_main_dir -gt 0 ]; then
    if [ $travis_flag -eq 0 ]; then
      time docker run --rm -v /sys/fs/cgroup:/sys/fs/cgroup:rw \
      -v $(pwd):/home/bench/benchexec_files:Z hrocha/benchexecrun \
      /bin/bash -c "cd release; python3 -m benchexec.benchexec --no-container ../$xml_doc_benchexec_name"
    else
      time docker run --rm -v /sys/fs/cgroup:/sys/fs/cgroup:rw \
      -v $(pwd):/home/bench/benchexec_files:Z hrocha/benchexecrun \
      /bin/bash -c "cd release; python3 -m benchexec.benchexec --no-container ../tests/regression_test/xml_doc_benchexec/$xml_doc_benchexec_name"
    fi

    if [ $travis_flag -eq 1 ]; then
      # Analyzing the results
      get_last_file_result=`ls -t release/results/*.txt | head -1`
      get_total_test_cases=`tail -n 10 $get_last_file_result | grep -oEi "Statistics:(.[^Files]*)" | grep -oe "[0-9]*"`
      get_total_success=`tail -n 10 $get_last_file_result | grep -oEi "Score:(.[^(]*)" | grep -oe "[0-9]*"`
      percent_tests=$(( ($get_total_success*100)/$get_total_test_cases ))

      if [ $travis_flag -eq 0 ]; then
        if [ $percent_tests -lt 20 ]; then
          echo ""
          echo "Sorry, the regression testing results were lower than expected, only: $percent_tests %"
          echo "Expected: 20%"
          exit 1
        fi
      else
        if [ $percent_tests -lt 25 ]; then
          echo ""
          echo "Sorry, the regression testing results were lower than expected, only: $percent_tests %"
          echo "Expected: 25%"
          exit 1
        fi
      fi
      echo "Regression testing results is: $percent_tests % of correct results from $get_total_test_cases test cases."
    fi

    echo ""
    echo "Benchmark Definition Result from: $xml_doc_benchexec_name"

    echo ""
    if [ $test_ce_format -eq 1 ]; then
      echo "Counterexample format validation result: "
      path_result_ce_f="release/result_ce.tmp"
      if [ -f $path_result_ce_f ]; then
        # count results
        get_ce_total_erros=$(cat "$path_result_ce_f" | wc -l)
        echo "$get_ce_total_erros  ERROR found :("
        cat $path_result_ce_f
      else
        "100% correct output."
      fi
    fi

    exit 0

  else
    echo "Sorry, you should be in the main directory of the tool"
    exit 1
  fi

else
  echo "Please, checkout the docker image available at https://github.com/hbgit/benchexecrun"
  exit 1
fi
