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

# Utility rule file for install-clang.

# Include the progress variables for this target.
include tools/clang/tools/driver/CMakeFiles/install-clang.dir/progress.make

tools/clang/tools/driver/CMakeFiles/install-clang: bin/clang
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/tools/driver && /usr/bin/cmake -DCMAKE_INSTALL_COMPONENT=clang -P /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/cmake_install.cmake

install-clang: tools/clang/tools/driver/CMakeFiles/install-clang
install-clang: tools/clang/tools/driver/CMakeFiles/install-clang.dir/build.make

.PHONY : install-clang

# Rule to build all files generated by this target.
tools/clang/tools/driver/CMakeFiles/install-clang.dir/build: install-clang

.PHONY : tools/clang/tools/driver/CMakeFiles/install-clang.dir/build

tools/clang/tools/driver/CMakeFiles/install-clang.dir/clean:
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/tools/driver && $(CMAKE_COMMAND) -P CMakeFiles/install-clang.dir/cmake_clean.cmake
.PHONY : tools/clang/tools/driver/CMakeFiles/install-clang.dir/clean

tools/clang/tools/driver/CMakeFiles/install-clang.dir/depend:
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1 /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang/tools/driver /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/tools/driver /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/tools/driver/CMakeFiles/install-clang.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tools/clang/tools/driver/CMakeFiles/install-clang.dir/depend

