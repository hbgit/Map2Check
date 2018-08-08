
set(REGRESSION_TYPE "FREE")
list(APPEND MAP2CHECK_REGRESSION_${REGRESSION_TYPE} "memleaks_test11_1_false-valid-free.i")
list(APPEND MAP2CHECK_REGRESSION_${REGRESSION_TYPE} "memleaks_test12_false-valid-free.i")
list(APPEND MAP2CHECK_REGRESSION_${REGRESSION_TYPE} "memleaks_test17_2_false-valid-free.i")
list(APPEND MAP2CHECK_REGRESSION_${REGRESSION_TYPE} "memleaks_test19_false-valid-free.i")
list(APPEND MAP2CHECK_REGRESSION_${REGRESSION_TYPE} "memleaks_test1_false-valid-free.i")
list(APPEND MAP2CHECK_REGRESSION_${REGRESSION_TYPE} "memleaks_test6_2_false-valid-free.i")
list(APPEND MAP2CHECK_REGRESSION_${REGRESSION_TYPE} "memleaks_test8_2_false-valid-free.i")


foreach(FILE ${MAP2CHECK_REGRESSION_${REGRESSION_TYPE}})  
  set(map2check_regression_${FILE} "${SVCOMP_BASE}ldv-memsafety/${FILE}")
endforeach()


#memleaks_test3_false-valid-free.i
