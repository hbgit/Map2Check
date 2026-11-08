#!/bin/sh
# Entry point of the Test-Comp reproduction image: a separate cgroup for
# BenchExec, as its doc/benchexec-in-container.md prescribes for cgroups v2.
# Without it BenchExec cannot limit or measure the runs it starts.
set -eu
mkdir -p /sys/fs/cgroup/init /sys/fs/cgroup/benchexec
echo $$ > /sys/fs/cgroup/init/cgroup.procs
for controller in $(cat /sys/fs/cgroup/cgroup.controllers); do
  echo "+$controller" > /sys/fs/cgroup/cgroup.subtree_control
  echo "+$controller" > /sys/fs/cgroup/benchexec/cgroup.subtree_control
done
exec "$@"
