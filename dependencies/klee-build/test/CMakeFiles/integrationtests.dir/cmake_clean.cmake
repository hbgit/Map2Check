file(REMOVE_RECURSE
  "Coverage/Output"
  "CXX/Output"
  "Concrete/Output"
  "Intrinsics/Output"
  "regression/Output"
  "Feature/Output"
  "Expr/Output"
  "Expr/Parser/Output"
  "Expr/Lexer/Output"
  "Solver/Output"
  "Programs/Output"
  "Dogfood/Output"
  "Runtime/Output"
  "Runtime/POSIX/Output"
  "Runtime/POSIX/SELinux/Output"
  "Runtime/Uclibc/Output"
  "CMakeFiles/integrationtests"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/integrationtests.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
