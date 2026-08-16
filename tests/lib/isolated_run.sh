# shellcheck shell=bash
# Shared runner for the CASTLE and Juliet evaluation harnesses.
#
#   run_isolated <raw_output_file> <outer_timeout_s> <command> [args...]
#
# Runs one map2check invocation in a private working directory, captures its
# combined output to a FILE, reaps every descendant, and returns the command's
# exit code. Nothing is written to stdout, so the caller reads the raw file.
#
# --- Why the output must NOT go through $( ) ---
#
# The obvious form is
#     output=$(timeout "$T" "$MAP2CHECK" ... 2>&1)
# and it deadlocks. Command substitution reads its pipe until EOF, and EOF only
# arrives once EVERY write end is closed. map2check spawns KLEE and LibFuzzer as
# grandchildren which inherit that pipe; when the outer `timeout` kills
# map2check, the grandchildren are orphaned but keep the write end open. The
# read then blocks forever.
#
# Observed directly on the v4 Juliet run, stalled 18 minutes on its first file:
#     pipe:[472046]
#       pid=113    fd=3  <- the harness subshell, blocked reading
#       pid=48538  fd=2  <- orphaned `timeout 48 klee`, write end held
#       pid=48539  fd=2  <- orphaned klee, write end held
# map2check itself was already gone. Redirecting to a file removes the
# dependency entirely: the harness never waits on a pipe, so an orphan can
# delay nothing.
#
# --- Why the descendants have to be reaped explicitly ---
#
# `timeout` signals only its direct child. Killing map2check therefore leaves
# KLEE running, burning CPU for the rest of the suite. Over a 21k-run baseline
# those accumulate until the machine is unusable. The private CWD makes them
# identifiable without guessing at process names or parsing pgids: any process
# whose cwd is inside this run's temp directory belongs to this run and to
# nothing else.
#
# `timeout -k` is also passed so a backend that catches SIGTERM without exiting
# is escalated to SIGKILL rather than waited on indefinitely. map2check applies
# the same escalation to its own internal KLEE/LibFuzzer invocations
# (tools.hpp:killGracePeriod); this is the outer belt to that inner braces.

# Seconds between SIGTERM and SIGKILL for the outer timeout.
: "${RUN_KILL_GRACE:=30}"

run_isolated() {
  local raw="$1" tmo="$2"
  shift 2

  local work rc=0 p cwd
  work=$(mktemp -d)

  # No pipe anywhere in this pipeline -- output lands straight in a file.
  ( cd "$work" && timeout -k "$RUN_KILL_GRACE" "$tmo" "$@" ) > "$raw" 2>&1 || rc=$?

  # Reap anything still rooted in this run's directory.
  for p in /proc/[0-9]*; do
    cwd=$(readlink "$p/cwd" 2>/dev/null) || continue
    case "$cwd" in
      "$work" | "$work"/*) kill -KILL "${p##*/}" 2>/dev/null || true ;;
    esac
  done

  rm -rf "$work"
  return "$rc"
}
