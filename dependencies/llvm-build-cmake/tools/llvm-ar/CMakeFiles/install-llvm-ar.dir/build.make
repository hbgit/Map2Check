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

# Utility rule file for install-llvm-ar.

# Include the progress variables for this target.
include tools/llvm-ar/CMakeFiles/install-llvm-ar.dir/progress.make

tools/llvm-ar/CMakeFiles/install-llvm-ar: bin/llvm-ar
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/llvm-ar && /usr/bin/cmake -DCMAKE_INSTALL_COMPONENT=llvm-ar -P /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/cmake_install.cmake

install-llvm-ar: tools/llvm-ar/CMakeFiles/install-llvm-ar
install-llvm-ar: tools/llvm-ar/CMakeFiles/install-llvm-ar.dir/build.make

.PHONY : install-llvm-ar

# Rule to build all files generated by this target.
tools/llvm-ar/CMakeFiles/install-llvm-ar.dir/build: install-llvm-ar

.PHONY : tools/llvm-ar/CMakeFiles/install-llvm-ar.dir/build

tools/llvm-ar/CMakeFiles/install-llvm-ar.dir/clean:
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/llvm-ar && $(CMAKE_COMMAND) -P CMakeFiles/install-llvm-ar.dir/cmake_clean.cmake
.PHONY : tools/llvm-ar/CMakeFiles/install-llvm-ar.dir/clean

tools/llvm-ar/CMakeFiles/install-llvm-ar.dir/depend:
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1 /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/llvm-ar /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/llvm-ar /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/llvm-ar/CMakeFiles/install-llvm-ar.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tools/llvm-ar/CMakeFiles/install-llvm-ar.dir/depend

