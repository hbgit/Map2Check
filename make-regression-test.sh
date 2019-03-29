#!/bin/sh

# Checking if testing is adopting travis service
xml_doc_benchexec_name="map2check_regression_test.xml"
travis_flag=0
if [ "$1" != "" ]; then
    xml_doc_benchexec_name="map2check_regression_test_travis.xml"
    travis_flag=1
fi

# Check if docker image is already build
# 1 - Build docker image available at https://github.com/hbgit/benchexecrun
has_docker_img=`docker images | grep -c "hrocha/benchexecrun"`
if [ $has_docker_img -gt 0 ]; then
  echo "Found benchexec image to run tests ..."
  is_main_dir=`ls | grep -c "make-regression-test.sh"`

  if [ $is_main_dir -gt 0 ]; then
    docker run --rm -v /sys/fs/cgroup:/sys/fs/cgroup:rw \
    -v $(pwd):/home/bench/benchexec_files:Z hrocha/benchexecrun \
    /bin/bash -c "cd release; python3 -m benchexec.benchexec --no-container ../tests/regression_test/xml_doc_benchexec/$xml_doc_benchexec_name"

    # Analyzing the results
    get_last_file_result=`ls -t release/results/*.txt | head -1`
    get_total_test_cases=`tail -n 10 $get_last_file_result | grep -oEi "Statistics:(.[^Files]*)" | grep -oe "[0-9][^ ] $" | grep -oe "[0-9]*"`
    get_total_success=`tail -n 10 $get_last_file_result | grep -oEi "Score:(.[^(]*)" | grep -oe "[0-9][^ ] $" | grep -oe "[0-9]*"`
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

    echo ""
    echo "Regression testing results is: $percent_tests % of correct results"
    exit 0

  else
    echo "Sorry, you should be in the main directory of the tool"
    exit 1
  fi

else
  echo "Please, checkout the docker image available at https://github.com/hbgit/benchexecrun"
  exit 1
fi
