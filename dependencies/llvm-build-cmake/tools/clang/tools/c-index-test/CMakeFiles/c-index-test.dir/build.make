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
include tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/depend.make

# Include the progress variables for this target.
include tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/progress.make

# Include the compile flags for this target's objects.
include tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/flags.make

tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/c-index-test.c.o: tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/flags.make
tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/c-index-test.c.o: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang/tools/c-index-test/c-index-test.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/c-index-test.c.o"
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/tools/c-index-test && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -std=gnu89 -o CMakeFiles/c-index-test.dir/c-index-test.c.o   -c /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang/tools/c-index-test/c-index-test.c

tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/c-index-test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/c-index-test.dir/c-index-test.c.i"
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/tools/c-index-test && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -std=gnu89 -E /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang/tools/c-index-test/c-index-test.c > CMakeFiles/c-index-test.dir/c-index-test.c.i

tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/c-index-test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/c-index-test.dir/c-index-test.c.s"
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/tools/c-index-test && /usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -std=gnu89 -S /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang/tools/c-index-test/c-index-test.c -o CMakeFiles/c-index-test.dir/c-index-test.c.s

tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/c-index-test.c.o.requires:

.PHONY : tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/c-index-test.c.o.requires

tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/c-index-test.c.o.provides: tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/c-index-test.c.o.requires
	$(MAKE) -f tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/build.make tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/c-index-test.c.o.provides.build
.PHONY : tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/c-index-test.c.o.provides

tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/c-index-test.c.o.provides.build: tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/c-index-test.c.o


# Object files for target c-index-test
c__index__test_OBJECTS = \
"CMakeFiles/c-index-test.dir/c-index-test.c.o"

# External object files for target c-index-test
c__index__test_EXTERNAL_OBJECTS =

bin/c-index-test: tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/c-index-test.c.o
bin/c-index-test: tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/build.make
bin/c-index-test: lib/libclang.so.3.8
bin/c-index-test: tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../../../bin/c-index-test"
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/tools/c-index-test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/c-index-test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/build: bin/c-index-test

.PHONY : tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/build

tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/requires: tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/c-index-test.c.o.requires

.PHONY : tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/requires

tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/clean:
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/tools/c-index-test && $(CMAKE_COMMAND) -P CMakeFiles/c-index-test.dir/cmake_clean.cmake
.PHONY : tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/clean

tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/depend:
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1 /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang/tools/c-index-test /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/tools/c-index-test /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tools/clang/tools/c-index-test/CMakeFiles/c-index-test.dir/depend

