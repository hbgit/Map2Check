file(REMOVE_RECURSE
  "CMakeFiles/scan-build"
  "../../../../bin/scan-build"
  "../../../../libexec/ccc-analyzer"
  "../../../../libexec/c++-analyzer"
  "../../../../share/man/man1/scan-build.1"
  "../../../../share/scan-build/scanview.css"
  "../../../../share/scan-build/sorttable.js"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/scan-build.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
