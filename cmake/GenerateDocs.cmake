# check if Doxygen is installed
find_package(Doxygen)
#find_package(Graphviz)

#if (DOXYGEN_FOUND AND GRAPHVIZ_FOUND)
if (DOXYGEN_FOUND AND BUILD_DOC)
    # set input and output files    
    set(DOXYGEN_IN ${PROJECT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/docs/
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )

else (DOXYGEN_FOUND AND BUILD_DOC)
  message("\nDoxygen not enabled to generate the documentation API. You need install doxygen or enable by -DBUILD_DOC=ON\n")
endif (DOXYGEN_FOUND AND BUILD_DOC)
