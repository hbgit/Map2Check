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

# Utility rule file for ClangDriverOptions.

# Include the progress variables for this target.
include tools/clang/include/clang/Driver/CMakeFiles/ClangDriverOptions.dir/progress.make

tools/clang/include/clang/Driver/CMakeFiles/ClangDriverOptions: tools/clang/include/clang/Driver/Options.inc


tools/clang/include/clang/Driver/Options.inc: tools/clang/include/clang/Driver/Options.inc.tmp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Updating Options.inc..."
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/include/clang/Driver && /usr/bin/cmake -E copy_if_different /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/include/clang/Driver/Options.inc.tmp /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/include/clang/Driver/Options.inc

tools/clang/include/clang/Driver/Options.inc.tmp: bin/llvm-tblgen
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang/include/clang/Driver/CC1Options.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang/include/clang/Driver/CLCompatOptions.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang/include/clang/Driver/Options.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/CodeGen/ValueTypes.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/IR/Attributes.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/IR/Intrinsics.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/IR/IntrinsicsAArch64.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/IR/IntrinsicsAMDGPU.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/IR/IntrinsicsARM.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/IR/IntrinsicsBPF.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/IR/IntrinsicsHexagon.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/IR/IntrinsicsMips.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/IR/IntrinsicsNVVM.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/IR/IntrinsicsPowerPC.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/IR/IntrinsicsSystemZ.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/IR/IntrinsicsWebAssembly.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/IR/IntrinsicsX86.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/IR/IntrinsicsXCore.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/Option/OptParser.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/Target/Target.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/Target/TargetCallingConv.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/Target/TargetItinerary.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/Target/TargetSchedule.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include/llvm/Target/TargetSelectionDAG.td
tools/clang/include/clang/Driver/Options.inc.tmp: /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang/include/clang/Driver/Options.td
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building Options.inc..."
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/include/clang/Driver && ../../../../../bin/llvm-tblgen -gen-opt-parser-defs -I /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang/include/clang/Driver -I /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/lib/Target -I /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/include /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang/include/clang/Driver/Options.td -o /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/include/clang/Driver/Options.inc.tmp

ClangDriverOptions: tools/clang/include/clang/Driver/CMakeFiles/ClangDriverOptions
ClangDriverOptions: tools/clang/include/clang/Driver/Options.inc
ClangDriverOptions: tools/clang/include/clang/Driver/Options.inc.tmp
ClangDriverOptions: tools/clang/include/clang/Driver/CMakeFiles/ClangDriverOptions.dir/build.make

.PHONY : ClangDriverOptions

# Rule to build all files generated by this target.
tools/clang/include/clang/Driver/CMakeFiles/ClangDriverOptions.dir/build: ClangDriverOptions

.PHONY : tools/clang/include/clang/Driver/CMakeFiles/ClangDriverOptions.dir/build

tools/clang/include/clang/Driver/CMakeFiles/ClangDriverOptions.dir/clean:
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/include/clang/Driver && $(CMAKE_COMMAND) -P CMakeFiles/ClangDriverOptions.dir/cmake_clean.cmake
.PHONY : tools/clang/include/clang/Driver/CMakeFiles/ClangDriverOptions.dir/clean

tools/clang/include/clang/Driver/CMakeFiles/ClangDriverOptions.dir/depend:
	cd /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1 /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-3.8.1/tools/clang/include/clang/Driver /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/include/clang/Driver /home/map2check/devel_tool/map_src_on_docker/dependencies/llvm-build-cmake/tools/clang/include/clang/Driver/CMakeFiles/ClangDriverOptions.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tools/clang/include/clang/Driver/CMakeFiles/ClangDriverOptions.dir/depend
