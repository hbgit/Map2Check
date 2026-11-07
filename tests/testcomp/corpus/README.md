# Test-Comp corpus manifests

Each `.tsv` lists the sv-benchmarks tasks a Test-Comp run executes, one per
line: `category`, `program` (relative to `sv-benchmarks/c/`), `data_model`
(ILP32 or LP64) and `expected_unreach`. The harness
(`tests/testcomp/run_testcomp_evaluation.sh`) reads it through `MANIFEST`.

None is hand-edited: each comes from a builder script, and rebuilding it
reproduces the committed file byte for byte. Checked on 2026-10-22 against
sv-benchmarks `7efe28dd2`, the commit pinned by Test-Comp 2026 (`testcomp26`
tag of bench-defs).

| Manifest | Tasks | Command | Commit |
|---|---|---|---|
| `cover-error-official.tsv` | 1902 | `build_corpus_official.py --property cover-error --total 0` | `0149e22f5` |
| `cover-branches-3500.tsv` | 3500 | `build_corpus_official.py --property cover-branches --total 3500` | `0149e22f5` |
| `cover-error-q400.tsv` | 1087 | `build_corpus.py --property cover-error --per-category 400` | `241adb230` |
| `cover-branches-q400.tsv` | 2765 | `build_corpus.py --property cover-branches --per-category 400` | `241adb230` |
| `cover-error.tsv` | 818 | `build_corpus.py --property cover-error --per-category 150` | `67b837834` |
| `cover-branches.tsv` | 1522 | `build_corpus.py --property cover-branches --per-category 150` | `67b837834` |
| `cover-error-q40.tsv` | 372 | `build_corpus.py --property cover-error --per-category 40` | `1f76c70d1` |
| `cover-branches-q40.tsv` | 480 | `build_corpus.py --property cover-branches --per-category 40` | `1f76c70d1` |

Run the scripts from `tests/testcomp/` with `--out <file>`, after linking
`tests/testcomp/bench/sv-benchmarks` to an sv-benchmarks checkout.

## The two builders

- `build_corpus.py` takes a **fixed quota per subcategory** (`--per-category`),
  so every family is equally represented. Its categories are the `.set` files
  of sv-benchmarks.
- `build_corpus_official.py` reads the categories, with their
  `<includesfile>`/`<excludesfile>`, from the **official Test-Comp 2026
  definition** (`official/bench-defs/testcov-validate-test-suites-gcc-unformatted.xml`),
  and gives each a quota **proportional** to its size (`--total`; 0 means
  every applicable task). This is the shape of the competition itself.

Both select inside a category deterministically, by a nested bisection order:
the first N tasks of a larger quota are the N of a smaller one, so raising a
quota adds tasks instead of replacing them. Both write the rows round-robin
across categories, so a run cut short by its deadline is still balanced.
