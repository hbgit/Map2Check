# Test suites written by Map2Check

With `--generate-test-suite`, Map2Check writes a Test-Comp test suite to
`./test-suite/` in the working directory, in the exchange format of
<https://gitlab.com/sosy-lab/software/test-format> (DTDs version 1.1). The
writer is `modules/frontend/test_suite/test_suite.cpp`; the decision of what
goes in is `emitTestSuite` in `modules/frontend/map2check.cpp`.

## Files

```
test-suite/
  metadata.xml
  testcase-1.xml
  testcase-2.xml
  ...
```

`metadata.xml` carries, in the order the DTD's sequence requires:
`sourcecodelang` (C), `producer` (`Map2Check <version>`), `specification`
(the property file's text), `programfile`, `programhash` (SHA-256),
`entryfunction`, `architecture` (`32bit`/`64bit`) and `creationtime` (UTC).

## A test case

```xml
<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<!DOCTYPE testcase PUBLIC "+//IDN sosy-lab.org//DTD test-format testcase 1.1//EN" "https://sosy-lab.org/test-format/testcase-1.1.dtd">
<testcase coversError="true">
  <input>42</input>
  <input>-7</input>
</testcase>
```

- The order is the whole contract: the n-th `<input>` is the value returned by
  the n-th `__VERIFIER_nondet_*` call of the execution.
- `coversError="true"` appears only on the case of a run that reached the
  error (Cover-Error); the DTD's default is false.
- Values are decimal integers; a `float` is printed with `%.9g` (nine
  significant digits read back to the same float), a `double` with `%lf`.
  `__VERIFIER_nondet_memory(p, n)` gives `n` inputs, one per byte, as the
  validators' harness reads it.
- Cases are numbered after the ones already in the directory, because the
  phases of one run (AFL++ and KLEE take turns) write into the same suite. A
  case whose inputs the suite already has is not written again.

## The assumption-pruned marker

```xml
<!DOCTYPE testcase PUBLIC "...">
<!-- map2check: assumption-pruned path -->
<testcase>
  <input>3</input>
</testcase>
```

A path that a failed assumption ended (`__VERIFIER_assume`,
`assume_abort_if_not`, or a program's own `abort()`, which NonDetPass turns
into pruning) still produces a test: KLEE ends it with `_exit(0)` and writes
its `.ktest`, carrying a one-byte `map2check_pruned` object that the reader
strips. Its vector covers the branches up to the assumption, so it belongs in
a Cover-Branches suite, but a deeper path is worth more.

The case is therefore written with the XML comment above. A comment is outside
the test format: TestCov and TestCoCa ignore it, and the case is validated
like any other. Map2Check reads it back when a later phase opens the same
suite (`TestSuiteWriter` constructor) and, once the suite is full, removes a
pruned case to make room for an unpruned one (`evictPrunedCase`). Without
that, on xcsp/AllInterval the first KLEE phase filled the suite with pruned
paths and the deep paths of the next phase found no room: coverage fell from
95% to 9%.

## How many cases

- Cover-Error: one, the violating execution.
- Cover-Branches: at most `kMaxBranchTestCases` (50). First the fuzzer's
  corpus, up to half the suite (on by default; `MAP2CHECK_FUZZER_SUITE=0`
  turns it off), then one case per KLEE path (`.ktest`), unpruned paths first.
  Pruned cases count toward neither share and give way when the suite is full.
  50 because larger suites (median 500 among the 116 that failed validation)
  were too large for TestCov to finish validating.

## Where the vectors come from

1. The runtime's nondet log `klee_log.csv`, one row per nondet call:
   `id;line;scope;function;step;value;type`, where `type` is the number of
   `enum NONDET_TYPE` (`modules/backend/library/header/Map2CheckTypes.h`:
   0 int, 1 char, 2 pointer, 3 ushort, 4 long, 5 unsigned, 6 ulong, 7 bool,
   8 uchar, 9 pchar, 10 size_t, 11 loff_t, 12 sector_t, 13 uint, 14 short,
   15 double, 16 float, 17 longlong, 18 ulonglong, 19 u8, 20 u16, 21 u32,
   22 one byte of nondet_memory).
2. When that log is missing -- an aborted KLEE state runs no exit handler, and
   past 1024 reads the KLEE runtime does not write it (issue #75) -- the
   violating state's `.ktest`, decoded by object name (`non_det_<type>`) in
   `modules/frontend/test_suite/ktest_reader.cpp`.
