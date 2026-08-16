# Test-Comp conformance corpus

Programs and property files used to check that Map2Check emits a test suite the
official validator accepts **and can execute to reach the bug**. That second half is
the point: structural validity of the XML is asserted separately by
`tests/integration/test_test_suite_emission.sh`, and a suite can be perfectly
well-formed and worth zero points.

## Layout

| Path | What it is |
|---|---|
| `cover-error-call.prp` | The Cover-Error property, as the competition states it |
| `programs/` | Small programs with a known outcome for `reach_error` |
| `expected.txt` | The expected TestCov verdict per program |
| `run_testcov.sh` | One program: generate a suite, zip it, ask TestCov |
| `run_testcov_suite.sh` | The whole corpus, compared against `expected.txt` |
| `NOTES-testcov-output.md` | TestCov's observed output contract, and why the parser looks the way it does |

## Running it

```sh
pip3 install testcov               # 3.7 at the time of writing
MAP2CHECK_PATH=<install prefix> bash tests/testcomp/run_testcov_suite.sh
```

Also needs `zip`, `gcc` and `lcov` on PATH. TestCov is the same tool the competition
uses; it comes from
[test-suite-validator](https://gitlab.com/sosy-lab/software/test-suite-validator).

## What the corpus is for

Each program isolates one thing the emitter can get wrong:

| Program | Isolates |
|---|---|
| `single_int.c` | The minimal path. A failure here means the mechanism is broken, not an edge case |
| `two_guards.c` | Ordering of two inputs — right values, wrong order reaches nothing |
| `no_input.c` | The empty test vector, which a naive emitter drops entirely |
| `char_input.c` | A non-`int` nondet type, i.e. a different branch of the value serializer |
| `loop_reads.c` | Four reads, where an off-by-one in count or order becomes visible |
| `unreachable.c` | **Negative control** — the error cannot be reached by any input |

`unreachable.c` earns its place: if it ever reports `COVERED`, the harness is lying
and no other result in this directory can be trusted. A gate that cannot fail is not
a gate.

## The manifest is bidirectional

`expected.txt` records one verdict per program, and `run_testcov_suite.sh` fails on
**any** mismatch — including a program that starts passing when the manifest says it
should not. That is deliberate. An unrecorded improvement looks exactly like a broken
harness, and the way to tell them apart is to make somebody change the line on
purpose, in the commit that caused it.

If a program does not reach its expected verdict, do not delete the line and do not
quietly relax it. Change it to the observed verdict and write down why in the commit
message. An honest `NOT_COVERED` with a reason is data; a deleted test is a blind
spot.

## Proof that the gate can fail

A gate nobody has seen fail is a gate nobody should trust. Reversing the order of
`<input>` elements in the emitter — a change that leaves every suite structurally
perfect and semantically worthless — was run against this corpus on 2026-08-16:

```
  PASS single_int.c       COVERED
  FAIL two_guards.c       expected COVERED, observed NOT_COVERED
  PASS no_input.c         COVERED
  PASS char_input.c       COVERED
  PASS unreachable.c      NOT_COVERED
  PASS loop_reads.c       NOT_COVERED
Results: 5 passed, 1 failed          (exit 1)
```

The gate goes red, as it must. But note **which** program caught it: only
`two_guards.c`. The others are structurally insensitive to ordering — one input each,
or none — and `loop_reads.c`, which has four and would also have caught it, is
currently undecided by the tool (finding K).

So the ordering guarantee rests on a single program today. Two consequences worth
keeping in mind: do not remove `two_guards.c` without adding another multi-input
program first, and fixing finding K would restore a second, independent witness.
