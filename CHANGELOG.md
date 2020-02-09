This is the changelog file for the Map2Check Tool.


Release v7.3.1.2019-11.27.140ba2d2 (2019-11-19)
===========================

- using LibFuzzer to provide random data as input to C programs to quickly expose “shallow” bugs, i.e., those that do not require complex data input;
- implementing a new runtime library and instrumentation approach to monitor for crashes, failing built-in assertions and pointer safety;
- adopting Crab-LLVM to infer invariants;
- exploiting a sequential approach with LibFuzzer and KLEE to check safety properties in a novel way;
- adopting MetaSMT as a wrapper around various SMT solvers, for instance, Boolector and Yices, previously not supported by our tool; and
- fixed a couple of bugs.


Release v7.1 - SV-COMP'18 (2017-11-17)
===========================

- minor changes to SVCOMP 2018
- futher details https://link.springer.com/chapter/10.1007/978-3-319-89963-3_28


Release v7.1 (2017-11-17)
===========================

- we added witness true
- we improved nondet functions
- we added files to svcomp'18
- fixed a couple of bugs
- general improvements to track memory address


Release v7.0 (2017-09-17)
===========================

- we adopt Clang to parse C code
- we use LLVM as intermediary representation to code instrumentation
- we use KLEE for symbolic execution
- added dockerfile to build Map2Check
- fix bugs related to counter-example generation
- general improvements to track memory address


Release v6 (2016-01-06)
==========================

- minor improvements to the tool.
- futher details at https://link.springer.com/chapter/10.1007/978-3-662-49674-9_64


Release v5 (2014-11-14)
==========================

- first Map2Check public release.
