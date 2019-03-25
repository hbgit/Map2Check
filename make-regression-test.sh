#!/bin/sh

# Check if docker image is already build
# 1 - Build docker image available at https://github.com/hbgit/benchexecrun
has_docker_img=`docker images | grep -c "hrocha/benchexecrun"`
if [ $has_docker_img -gt 0 ]; then
  echo "Found benchexec image to run tests ..."
  is_main_dir=`ls | grep -c "make-regression-test.sh"`

  if [ $is_main_dir -gt 0 ]; then
    docker run --rm -v /sys/fs/cgroup:/sys/fs/cgroup:rw \
    -v $(pwd):/home/bench/benchexec_files:Z hrocha/benchexecrun \
    /bin/bash -c "cd release; python3 -m benchexec.benchexec --no-container ../tests/regression_test/xml_doc_benchexec/map2check_regression_test.xml"
  else
    echo "Sorry, you should be in the main directory of the tool"
  fi

else
  echo "Please, checkout the docker image available at https://github.com/hbgit/benchexecrun"
fi
