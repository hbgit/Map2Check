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
include lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/depend.make

# Include the progress variables for this target.
include lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/progress.make

# Include the compile flags for this target's objects.
include lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/flags.make

lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.o: lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/flags.make
lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.o: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/lib/ExecutionEngine/MCJIT/MCJIT.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.o"
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/ExecutionEngine/MCJIT && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.o -c /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/lib/ExecutionEngine/MCJIT/MCJIT.cpp

lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.i"
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/ExecutionEngine/MCJIT && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/lib/ExecutionEngine/MCJIT/MCJIT.cpp > CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.i

lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.s"
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/ExecutionEngine/MCJIT && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/lib/ExecutionEngine/MCJIT/MCJIT.cpp -o CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.s

lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.o.requires:

.PHONY : lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.o.requires

lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.o.provides: lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.o.requires
	$(MAKE) -f lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/build.make lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.o.provides.build
.PHONY : lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.o.provides

lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.o.provides.build: lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.o


# Object files for target LLVMMCJIT
LLVMMCJIT_OBJECTS = \
"CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.o"

# External object files for target LLVMMCJIT
LLVMMCJIT_EXTERNAL_OBJECTS =

lib/libLLVMMCJIT.a: lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.o
lib/libLLVMMCJIT.a: lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/build.make
lib/libLLVMMCJIT.a: lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library ../../libLLVMMCJIT.a"
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/ExecutionEngine/MCJIT && $(CMAKE_COMMAND) -P CMakeFiles/LLVMMCJIT.dir/cmake_clean_target.cmake
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/ExecutionEngine/MCJIT && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/LLVMMCJIT.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/build: lib/libLLVMMCJIT.a

.PHONY : lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/build

lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/requires: lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/MCJIT.cpp.o.requires

.PHONY : lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/requires

lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/clean:
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/ExecutionEngine/MCJIT && $(CMAKE_COMMAND) -P CMakeFiles/LLVMMCJIT.dir/cmake_clean.cmake
.PHONY : lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/clean

lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/depend:
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1 /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/lib/ExecutionEngine/MCJIT /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/ExecutionEngine/MCJIT /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lib/ExecutionEngine/MCJIT/CMakeFiles/LLVMMCJIT.dir/depend

