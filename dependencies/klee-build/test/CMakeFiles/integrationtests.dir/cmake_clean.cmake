file(REMOVE_RECURSE
  "CXX/Output"
  "Concrete/Output"
  "Coverage/Output"
  "Dogfood/Output"
  "Expr/Output"
  "Expr/Lexer/Output"
  "Expr/Parser/Output"
  "Feature/Output"
  "Intrinsics/Output"
  "Programs/Output"
  "Runtime/Output"
  "Runtime/POSIX/Output"
  "Runtime/POSIX/SELinux/Output"
  "Runtime/Uclibc/Output"
  "Solver/Output"
  "regression/Output"
  "CMakeFiles/integrationtests"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/integrationtests.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
