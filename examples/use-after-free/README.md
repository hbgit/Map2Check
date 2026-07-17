# Use-After-Free Example (CVE-2024-9680 inspired)

This example illustrates a simplified use-after-free vulnerability inspired by
the Animation timelines bug in Firefox v131.0.2
([CVE-2024-9680](https://nvd.nist.gov/vuln/detail/CVE-2024-9680)).

The program allocates memory with `malloc`, stores a value, frees the pointer,
and then dereferences it — a classic temporal memory safety violation
(CWE-416).

## Running with Map2Check

```bash
$ ./map2check --memtrack sample_use_after_free.c
```

Expected output: `VERIFICATION FAILED` with `FALSE-DEREF: Reference to pointer was lost`,
which corresponds to CWE-416 (Use-After-Free).

This example is discussed in the Map2Check SBSeg 2026 paper as Listing 1
(*Motivating Example* section).
