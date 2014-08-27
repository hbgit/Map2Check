file code_samples/pre_test-0019_1_false-valid-memtrack.c: Parsing
Converting
Type-checking pre_test-0019_1_false-valid-memtrack
Generating GOTO Program
Pointer Analysis
Adding Pointer Checks
Claim 1:
  file code_samples/pre_test-0019_1_false-valid-memtrack.c line 14 function alloc_data
  dereference failure: invalid pointer
  !(INVALID-POINTER(data.lo))

Claim 2:
  file code_samples/pre_test-0019_1_false-valid-memtrack.c line 15 function alloc_data
  dereference failure: invalid pointer
  !(INVALID-POINTER(data.hi))

Claim 3:
  file code_samples/pre_test-0019_1_false-valid-memtrack.c line 24 function free_data
  dereference failure: invalid pointer
  !(INVALID-POINTER(lo))

Claim 4:
  file code_samples/pre_test-0019_1_false-valid-memtrack.c line 24 function free_data
  dereference failure: invalid pointer
  !(INVALID-POINTER(hi))

Claim 5:
  file code_samples/pre_test-0019_1_false-valid-memtrack.c line 26 function free_data
  dereference failure: invalid pointer
  !(INVALID-POINTER((void *)lo))

Claim 6:
  file code_samples/pre_test-0019_1_false-valid-memtrack.c line 27 function free_data
  dereference failure: invalid pointer
  !(INVALID-POINTER((void *)hi))

