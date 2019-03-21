How to execute regression testing:

1) Build docker image available at https://github.com/hbgit/benchexecrun

2) Run a test with the container from docker image:
docker run -it --rm -v /sys/fs/cgroup:/sys/fs/cgroup:rw -v $(pwd):/benchexecrun/ hrocha/benchexec /bin/bash -c "runexec --no-container echo Test"

3) If step 2 is okay, then you could run the regression testing inside of the Map2Check binary directory (e.g., release):
$ cd Map2Check

$ ls
build  cmake  CMakeLists.txt  CODE_OF_CONDUCT.md  Dockerfile  docs  LICENSE  make-release.sh  make-test.sh  modules  README.md  release  tests  utils

$ docker run -it --rm -v /sys/fs/cgroup:/sys/fs/cgroup:rw -v $(pwd):/home/bench/benchexec_files:Z hrocha/benchexec /bin/bash -c "cd release; python3 -m benchexec.benchexec ../tests/regression_test/xml_doc_benchexec/map2check_regression_test.xml"
