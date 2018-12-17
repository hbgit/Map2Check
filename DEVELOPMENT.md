# Environment

## Dependencies
* Ubuntu (or derivatives)
* build-essential: `apt install build-essential`
* cmake: `apt install cmake`
* curl: `apt install curl`
* boost: `apt install libboost-all-dev`

## Build
1. Clone or save this repository.
2. Run [make-release](make-release.sh) script.

# Code Style
Currently is based on the [Google code style](https://google.github.io/styleguide/cppguide.html). The easiest way is to run `clang-format` with the [.clang-format](.clang-format) file.

# Project structure
Map2Check has 3 main modules used: [Frontend](modules/frontend/), [Pass-Backend](modules/backend/pass/), [Library-Backend](modules/backend/library/)

## Frontend
It is responsible to get inputs from users, interface between pass and library, generate counter-example and witness files, interface for clang, etc. 

Notes: 
* To add a new analysis or new pass, just look at the [caller class implementation](modules/frontend/caller.cpp).
* To add a new option, just look at the `main` from [here](modules/frontend/map2check.cpp) .

## Pass-Backend
Contains all llvm pass/instrumentation/sanitazers used on our analysis.

## Library-Backend
Contains all implementation of the instrumented functions.

Notes:
* To create a new analysis mode, just implement the [AnalysisMode](modules/backend/library/header/AnalysisMode.h) using the [Map2CheckFunctions](modules/backend/library/header/Map2CheckFunctions.h) API. 