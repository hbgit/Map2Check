# TestCov 3.7 output contract, as observed

Recorded 2026-08-16 inside `map2check-dev`, TestCov 3.7 from PyPI, invoked as
`testcov --test-suite <zip> --no-isolation -64 --goal cover-error-call.prp <program.c>`.

Both runs used the same program (`programs/two_guards.c`, error reachable only at
`a == 42 && b == 7`). The first used the suite Map2Check generated; the second used
a byte-identical suite with the two input values replaced by `1` and `2`, so it is
structurally valid and covers nothing.

## Suite that covers the error

Exit code: **0**

```
⏳ Executing tests 1/1
WARNING:suite_validation:Unmatched occurence of reach_error: extern void reach_error(void);
INFO:suite_validation:Stopping. Error found for test testcase-1.xml
✔️  Done!
INFO:suite_validation:Function-covering test case written to output/covering-test.c

---Results---
Tests run: 1
Tests in suite: 1
Coverage: 100.0%
Number of goals: 1
Result: TRUE
```

## Valid suite that does not cover the error

Exit code: **0**

```
⏳ Executing tests 1/1
WARNING:suite_validation:Unmatched occurence of reach_error: extern void reach_error(void);
✔️  Done!

---Results---
Tests run: 1
Tests in suite: 1
Coverage: 0.0%
Number of goals: 1
Result: UNKNOWN
```

## Consequence

**The exit code is 0 in both cases.** A gate that keyed off `$?` would pass
unconditionally and be worth nothing — which is the single most important reason
this file exists. `testcov_verdict()` in `run_testcov.sh` matches the `Result:` line
instead:

| observed line | verdict |
|---|---|
| `Result: TRUE`    | `COVERED` |
| `Result: UNKNOWN` | `NOT_COVERED` |
| neither present   | `ERROR` |

Falling through to `ERROR` rather than to `NOT_COVERED` is deliberate. If a TestCov
upgrade changes the wording, the gate fails loudly instead of silently reporting that
nothing covers anything — a green-but-vacuous gate is worse than a red one.

## Two things that look like problems and are not

- `WARNING: Unmatched occurence of reach_error` appears in **both** runs, including
  the successful one. It refers to the `extern` declaration line, not to the call.
  Do not treat it as a failure signal.
- `WARNING: Not plotting coverage statistics: No module named 'matplotlib'` is
  cosmetic. Installing matplotlib in CI would buy nothing.

## Environment notes

- `--no-isolation` is required wherever cgroups are unavailable (GitHub Actions
  runners). It disables TestCov's own resource limits, so the caller must impose a
  timeout.
- `-64` is required: TestCov defaults to `-m32`, while Map2Check compiles for the
  host's 64-bit model. A mismatch shows up as a suite that mysteriously covers
  nothing rather than as an error.
- `--test-suite` takes a **zip**, while the competition consumes a directory. The
  driver zips; the tool must not.
- Packages needed beyond the base image: `python3-pip`, `zip`, `gcc`, `lcov`.
