# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake

# Include any dependencies generated for this target.
include utils/FileCheck/CMakeFiles/FileCheck.dir/depend.make

# Include the progress variables for this target.
include utils/FileCheck/CMakeFiles/FileCheck.dir/progress.make

# Include the compile flags for this target's objects.
include utils/FileCheck/CMakeFiles/FileCheck.dir/flags.make

utils/FileCheck/CMakeFiles/FileCheck.dir/FileCheck.cpp.o: utils/FileCheck/CMakeFiles/FileCheck.dir/flags.make
utils/FileCheck/CMakeFiles/FileCheck.dir/FileCheck.cpp.o: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/utils/FileCheck/FileCheck.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object utils/FileCheck/CMakeFiles/FileCheck.dir/FileCheck.cpp.o"
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/utils/FileCheck && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/FileCheck.dir/FileCheck.cpp.o -c /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/utils/FileCheck/FileCheck.cpp

utils/FileCheck/CMakeFiles/FileCheck.dir/FileCheck.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/FileCheck.dir/FileCheck.cpp.i"
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/utils/FileCheck && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/utils/FileCheck/FileCheck.cpp > CMakeFiles/FileCheck.dir/FileCheck.cpp.i

utils/FileCheck/CMakeFiles/FileCheck.dir/FileCheck.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/FileCheck.dir/FileCheck.cpp.s"
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/utils/FileCheck && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/utils/FileCheck/FileCheck.cpp -o CMakeFiles/FileCheck.dir/FileCheck.cpp.s

utils/FileCheck/CMakeFiles/FileCheck.dir/FileCheck.cpp.o.requires:

.PHONY : utils/FileCheck/CMakeFiles/FileCheck.dir/FileCheck.cpp.o.requires

utils/FileCheck/CMakeFiles/FileCheck.dir/FileCheck.cpp.o.provides: utils/FileCheck/CMakeFiles/FileCheck.dir/FileCheck.cpp.o.requires
	$(MAKE) -f utils/FileCheck/CMakeFiles/FileCheck.dir/build.make utils/FileCheck/CMakeFiles/FileCheck.dir/FileCheck.cpp.o.provides.build
.PHONY : utils/FileCheck/CMakeFiles/FileCheck.dir/FileCheck.cpp.o.provides

utils/FileCheck/CMakeFiles/FileCheck.dir/FileCheck.cpp.o.provides.build: utils/FileCheck/CMakeFiles/FileCheck.dir/FileCheck.cpp.o


# Object files for target FileCheck
FileCheck_OBJECTS = \
"CMakeFiles/FileCheck.dir/FileCheck.cpp.o"

# External object files for target FileCheck
FileCheck_EXTERNAL_OBJECTS =

bin/FileCheck: utils/FileCheck/CMakeFiles/FileCheck.dir/FileCheck.cpp.o
bin/FileCheck: utils/FileCheck/CMakeFiles/FileCheck.dir/build.make
bin/FileCheck: lib/libLLVMSupport.a
bin/FileCheck: utils/FileCheck/CMakeFiles/FileCheck.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../bin/FileCheck"
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/utils/FileCheck && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/FileCheck.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
utils/FileCheck/CMakeFiles/FileCheck.dir/build: bin/FileCheck

.PHONY : utils/FileCheck/CMakeFiles/FileCheck.dir/build

utils/FileCheck/CMakeFiles/FileCheck.dir/requires: utils/FileCheck/CMakeFiles/FileCheck.dir/FileCheck.cpp.o.requires

.PHONY : utils/FileCheck/CMakeFiles/FileCheck.dir/requires

utils/FileCheck/CMakeFiles/FileCheck.dir/clean:
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/utils/FileCheck && $(CMAKE_COMMAND) -P CMakeFiles/FileCheck.dir/cmake_clean.cmake
.PHONY : utils/FileCheck/CMakeFiles/FileCheck.dir/clean

utils/FileCheck/CMakeFiles/FileCheck.dir/depend:
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1 /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/utils/FileCheck /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/utils/FileCheck /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/utils/FileCheck/CMakeFiles/FileCheck.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : utils/FileCheck/CMakeFiles/FileCheck.dir/depend

