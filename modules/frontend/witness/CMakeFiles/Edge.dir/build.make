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
CMAKE_SOURCE_DIR = /home/map2check/devel_tool/map_src_on_docker

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/map2check/devel_tool/map_src_on_docker

# Include any dependencies generated for this target.
include modules/frontend/witness/CMakeFiles/Edge.dir/depend.make

# Include the progress variables for this target.
include modules/frontend/witness/CMakeFiles/Edge.dir/progress.make

# Include the compile flags for this target's objects.
include modules/frontend/witness/CMakeFiles/Edge.dir/flags.make

modules/frontend/witness/CMakeFiles/Edge.dir/edge.cpp.o: modules/frontend/witness/CMakeFiles/Edge.dir/flags.make
modules/frontend/witness/CMakeFiles/Edge.dir/edge.cpp.o: modules/frontend/witness/edge.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/map2check/devel_tool/map_src_on_docker/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object modules/frontend/witness/CMakeFiles/Edge.dir/edge.cpp.o"
	cd /home/map2check/devel_tool/map_src_on_docker/modules/frontend/witness && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Edge.dir/edge.cpp.o -c /home/map2check/devel_tool/map_src_on_docker/modules/frontend/witness/edge.cpp

modules/frontend/witness/CMakeFiles/Edge.dir/edge.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Edge.dir/edge.cpp.i"
	cd /home/map2check/devel_tool/map_src_on_docker/modules/frontend/witness && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/map2check/devel_tool/map_src_on_docker/modules/frontend/witness/edge.cpp > CMakeFiles/Edge.dir/edge.cpp.i

modules/frontend/witness/CMakeFiles/Edge.dir/edge.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Edge.dir/edge.cpp.s"
	cd /home/map2check/devel_tool/map_src_on_docker/modules/frontend/witness && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/map2check/devel_tool/map_src_on_docker/modules/frontend/witness/edge.cpp -o CMakeFiles/Edge.dir/edge.cpp.s

modules/frontend/witness/CMakeFiles/Edge.dir/edge.cpp.o.requires:

.PHONY : modules/frontend/witness/CMakeFiles/Edge.dir/edge.cpp.o.requires

modules/frontend/witness/CMakeFiles/Edge.dir/edge.cpp.o.provides: modules/frontend/witness/CMakeFiles/Edge.dir/edge.cpp.o.requires
	$(MAKE) -f modules/frontend/witness/CMakeFiles/Edge.dir/build.make modules/frontend/witness/CMakeFiles/Edge.dir/edge.cpp.o.provides.build
.PHONY : modules/frontend/witness/CMakeFiles/Edge.dir/edge.cpp.o.provides

modules/frontend/witness/CMakeFiles/Edge.dir/edge.cpp.o.provides.build: modules/frontend/witness/CMakeFiles/Edge.dir/edge.cpp.o


Edge: modules/frontend/witness/CMakeFiles/Edge.dir/edge.cpp.o
Edge: modules/frontend/witness/CMakeFiles/Edge.dir/build.make

.PHONY : Edge

# Rule to build all files generated by this target.
modules/frontend/witness/CMakeFiles/Edge.dir/build: Edge

.PHONY : modules/frontend/witness/CMakeFiles/Edge.dir/build

modules/frontend/witness/CMakeFiles/Edge.dir/requires: modules/frontend/witness/CMakeFiles/Edge.dir/edge.cpp.o.requires

.PHONY : modules/frontend/witness/CMakeFiles/Edge.dir/requires

modules/frontend/witness/CMakeFiles/Edge.dir/clean:
	cd /home/map2check/devel_tool/map_src_on_docker/modules/frontend/witness && $(CMAKE_COMMAND) -P CMakeFiles/Edge.dir/cmake_clean.cmake
.PHONY : modules/frontend/witness/CMakeFiles/Edge.dir/clean

modules/frontend/witness/CMakeFiles/Edge.dir/depend:
	cd /home/map2check/devel_tool/map_src_on_docker && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/map2check/devel_tool/map_src_on_docker /home/map2check/devel_tool/map_src_on_docker/modules/frontend/witness /home/map2check/devel_tool/map_src_on_docker /home/map2check/devel_tool/map_src_on_docker/modules/frontend/witness /home/map2check/devel_tool/map_src_on_docker/modules/frontend/witness/CMakeFiles/Edge.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : modules/frontend/witness/CMakeFiles/Edge.dir/depend

