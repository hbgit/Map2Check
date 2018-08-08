# TODO: rename deref to SVCOMP
set(SVCOMP_BASE "https://raw.githubusercontent.com/sosy-lab/sv-benchmarks/master/c/")
list(APPEND MAP2CHECK_REGRESSION "DEREF")

foreach(R ${MAP2CHECK_REGRESSION})
  include(${PROJECT_SOURCE_DIR}/cmake/DownloadRegression${R}.cmake)
  foreach(F ${MAP2CHECK_REGRESSION_${R}})
    file(DOWNLOAD ${map2check_regression_${F}} tests/${R}/${F} SHOW_PROGRESS)
  endforeach()
endforeach()

install(DIRECTORY ${PROJECT_BINARY_DIR}/tests/ DESTINATION tests/)
