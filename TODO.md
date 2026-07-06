# TODO — OpenSSF Badge: `static_analysis_fixed` criterion not met

## Status

**Not confirmed / Not Met** — contradicts the claim in `docs/migration/1.5-openssf-badge.md`.

## Criterion

> All medium and higher severity exploitable vulnerabilities discovered with static code analysis MUST be fixed in a timely way after they are confirmed. [`static_analysis_fixed`]

## Evidence

Verified directly against the current codebase (not just the docs) — the following static-analysis findings are still present:

- [ ] **CWE-119 buffer overflow (`strcpy`)** — `modules/frontend/map2check.cpp:93,102,111` (clang-tidy `clang-analyzer-security.insecureAPI.strcpy`) — still present, unchanged.
- [ ] **Off-by-one / OOB write** — `modules/backend/library/lib/BTree.c:274-276` — loop `i < ORDER*2+1` writes to `children[i]` one past the array's bound (cppcheck `arrayIndexOutOfBounds`) — confirmed still present.
- [ ] **Dangling pointer to local VLA** — `modules/backend/library/lib/NonDetGeneratorKlee.c:41,47` — declares `char string[length]` and does `return string;` (cppcheck `returnDanglingLifetime`) — still present.
- [ ] **Shift UB / uninitialized variables** — `NonDetGeneratorLibFuzzy.c:103`, `AllocationLog.c:56`, `NonDetLog.c:70`, `ContainerBTree.c:23` — documented as pending in `docs/migration-schedule.md` (Phase 2.0).

## Confirmed?

Yes — all findings are precisely documented (file, line, tool, CWE) in `docs/migration-schedule.md` since commit `c320918d` (2026-06-14), ~3 weeks ago.

## Fixed in a timely manner?

No. All 12 items in Phase 2.0 of `docs/migration-schedule.md` remain **unchecked (☐)**, including the 5 items the project itself flagged as priority ("must be prioritized before any new implementation"). Other commits (docs, CI, tooling) landed in the meantime without addressing these.

## Verdict

**Not Met.** This contradicts the claim in `docs/migration/1.5-openssf-badge.md` ("medium+ issues are tracked via GitHub Issues and fixed before release") — in practice they are tracked but **not fixed**. If the bestpractices.dev badge profile marks `static_analysis_fixed` as "Met", it should be reverted to "?" or "Unmet" until at least items 2.0.1–2.0.5 (security/UB) are fixed.

---

# TODO — OpenSSF Badge: `dynamic_analysis_unsafe` criterion not met

## Status

**Not Met** (weak/questionable at best).

## Criterion

> It is SUGGESTED that if the software produced by the project includes software written using a memory-unsafe language (e.g., C or C++), then at least one dynamic tool (e.g., a fuzzer or web application scanner) be routinely used in combination with a mechanism to detect memory safety problems such as buffer overwrites. If the project does not produce software written in a memory-unsafe language, choose "not applicable" (N/A). [`dynamic_analysis_unsafe`]

Map2Check is written in C/C++ (memory-unsafe), so **N/A does not apply** — the criterion must actually be met.

## What exists

- ASan/UBSan (a memory-safety detection mechanism) is configured and runs in the `sanitizer-tests` job — this covers the "mechanism to detect memory safety problems such as buffer overwrites" half of the criterion.

## What's missing — the "dynamic tool (e.g., a fuzzer)" half

- [ ] `LibFuzzer` is **explicitly disabled** (`-DSKIP_LIB_FUZZER=ON`) in **all 3 CI jobs** (`build-and-test`, `static-analysis`, and even `sanitizer-tests`) — ASan only runs against the **7 fixed unit tests** (`ctest`), not against a fuzzer exploring inputs.
- [ ] `NonDetGeneratorLibFuzzy.c` and the bundled `libclang_rt.fuzzer*` are **LibFuzzer as a Map2Check product feature** (used to generate inputs for the *user's C programs* being analyzed) — not dogfooding/self-fuzzing of Map2Check's own source code.
- [ ] No fuzzing harness for Map2Check's own code (`LLVMFuzzerTestOneInput`), no corpus, no OSS-Fuzz integration found.
- [ ] The only real self-fuzzing initiative on the roadmap is the planned **AFL++** integration (`docs/migration-schedule.md`, Phase 3, items 3.1.1–3.1.4) — entirely **not started** (☐), scheduled for December/January.

## Verdict

**Not Met**, or at best weak/questionable. Running a fixed set of 7 unit tests under ASan is valid dynamic execution (already counted toward the more generic `dynamic_analysis` criterion, confirmed separately), but it does not substitute for a fuzzer/scanner broadly exploring inputs — and the project's own fuzzing tool is turned off in every CI job.

---

# TODO — OpenSSF Badge: `dynamic_analysis_enable_assertions` criterion questionable

## Status

**Met in form only, not in practice.**

## Criterion

> It is SUGGESTED that the project use a configuration for at least some dynamic analysis (such as testing or fuzzing) which enables many assertions. In many cases these assertions should not be enabled in production builds. [`dynamic_analysis_enable_assertions`]

## Part 1 — the build configuration does not disable assertions

Checked all 3 CI jobs (`build-and-test`, `static-analysis`, `sanitizer-tests`) and the root `CMakeLists.txt`: **none set `CMAKE_BUILD_TYPE=Release`/`MinSizeRel`** (which is what makes CMake inject `-DNDEBUG`, disabling `assert()`). Since `CMAKE_BUILD_TYPE` is left empty/unspecified throughout CI, `assert()` would remain active if present in the code. This technically satisfies the configuration requirement.

## Part 2 — but how many assertions actually exist to be "enabled"?

Searched for real `assert()` (standard C/C++) and `BOOST_ASSERT()` usage — the latter explicitly recognized by `.clang-tidy`'s own config (`bugprone-assert-side-effect.AssertMacros: 'assert,BOOST_ASSERT'`) — across all of `modules/`:

- [ ] `assert(...)` — **0 occurrences**
- [ ] `BOOST_ASSERT(...)` — **0 occurrences**

The only "assert"-named symbols in the codebase are Map2Check's own domain functions (`map2check_assert`, `__VERIFIER_assert`, `write_property_assert`) — these implement the tool's *verification feature* (checking asserts in the user's analyzed program), not a defensive-programming practice inside Map2Check's own source.

## Verdict

**Questionable (Met in form only).** The test configuration doesn't strip assertions, but that's moot because **there are no real assertions in the code to enable**. Calling this "Met" would be a very literal reading — in practice, the project doesn't use `assert()`/`BOOST_ASSERT()` as a dynamic-analysis mechanism at all. `.clang-tidy` is already pre-configured to recognize these macros, but there's no actual usage yet.

---

# TODO — OpenSSF Badge: `dynamic_analysis_fixed` criterion not met

## Status

**Not Met** — same pattern as `static_analysis_fixed`, now on the dynamic-analysis side.

## Criterion

> All medium and higher severity exploitable vulnerabilities discovered with dynamic code analysis MUST be fixed in a timely way after they are confirmed. [`dynamic_analysis_fixed`]

## Evidence — medium+ vulnerability confirmed by dynamic analysis (not just static)

- [ ] **UBSan** triggered the same `BTree.c:276` off-by-one bug **at runtime** — `"index 34 out of bounds for type 'struct B_TREE_PAGE *[34]'"` — confirmed in **two real tests**: `BTreeTest` and `ContainerBTreeTest`. This is not theoretical: it's an out-of-bounds write (`children[34]` in a size-34 array) actually triggered during execution — an exploitable vulnerability class (CWE-787 / off-by-one write).
- [ ] **ASan** confirmed memory leaks in `AllocationLogTest` (lines 75, 87) and `AllocationLog.c:89` — lower severity (leaks in test code, not the library itself), but also catalogued and unfixed.

## Fixed in a timely manner?

No:
- `docs/migration-schedule.md:158` — item `2.0.1` (BTree off-by-one, 🔴 UBSan Error) remains **unchecked (☐)**.
- `docs/migration-schedule.md:167` — item `2.0.10` (test leaks, 🟢 ASan Leak) remains **unchecked (☐)**.
- Telling detail: `.github/workflows/ci.yml` documents this itself in a comment on the `sanitizer-tests` job:
  ```yaml
  # halt_on_error=0: report pre-existing UB issues without blocking CI
  # Known: BTree.c:276 off-by-one (tracked for fix in Phase 2)
  ```
  In other words, CI was **deliberately configured** (`halt_on_error=0`, `detect_leaks=0`) to avoid failing because of an already-confirmed, unfixed vulnerability — the opposite of what this criterion requires.

## Verdict

**Not Met.** Identical pattern to `static_analysis_fixed`: the vulnerability is detected and precisely documented, but remains unfixed for ~3 weeks, and CI was tuned to tolerate it rather than block on it.
