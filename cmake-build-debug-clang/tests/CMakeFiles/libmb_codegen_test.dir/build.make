# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/ego/apps/clion/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/ego/apps/clion/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ego/projects/codegen

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ego/projects/codegen/cmake-build-debug-clang

# Include any dependencies generated for this target.
include tests/CMakeFiles/libmb_codegen_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include tests/CMakeFiles/libmb_codegen_test.dir/compiler_depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/libmb_codegen_test.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/libmb_codegen_test.dir/flags.make

tests/CMakeFiles/libmb_codegen_test.dir/codegen_test.cpp.o: tests/CMakeFiles/libmb_codegen_test.dir/flags.make
tests/CMakeFiles/libmb_codegen_test.dir/codegen_test.cpp.o: ../tests/codegen_test.cpp
tests/CMakeFiles/libmb_codegen_test.dir/codegen_test.cpp.o: tests/CMakeFiles/libmb_codegen_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ego/projects/codegen/cmake-build-debug-clang/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/CMakeFiles/libmb_codegen_test.dir/codegen_test.cpp.o"
	cd /home/ego/projects/codegen/cmake-build-debug-clang/tests && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT tests/CMakeFiles/libmb_codegen_test.dir/codegen_test.cpp.o -MF CMakeFiles/libmb_codegen_test.dir/codegen_test.cpp.o.d -o CMakeFiles/libmb_codegen_test.dir/codegen_test.cpp.o -c /home/ego/projects/codegen/tests/codegen_test.cpp

tests/CMakeFiles/libmb_codegen_test.dir/codegen_test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/libmb_codegen_test.dir/codegen_test.cpp.i"
	cd /home/ego/projects/codegen/cmake-build-debug-clang/tests && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ego/projects/codegen/tests/codegen_test.cpp > CMakeFiles/libmb_codegen_test.dir/codegen_test.cpp.i

tests/CMakeFiles/libmb_codegen_test.dir/codegen_test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/libmb_codegen_test.dir/codegen_test.cpp.s"
	cd /home/ego/projects/codegen/cmake-build-debug-clang/tests && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ego/projects/codegen/tests/codegen_test.cpp -o CMakeFiles/libmb_codegen_test.dir/codegen_test.cpp.s

# Object files for target libmb_codegen_test
libmb_codegen_test_OBJECTS = \
"CMakeFiles/libmb_codegen_test.dir/codegen_test.cpp.o"

# External object files for target libmb_codegen_test
libmb_codegen_test_EXTERNAL_OBJECTS =

tests/libmb_codegen_test: tests/CMakeFiles/libmb_codegen_test.dir/codegen_test.cpp.o
tests/libmb_codegen_test: tests/CMakeFiles/libmb_codegen_test.dir/build.make
tests/libmb_codegen_test: _deps/libmb-build/liblibmb.a
tests/libmb_codegen_test: liblibmb_codegen.a
tests/libmb_codegen_test: _deps/libmb-build/liblibmb.a
tests/libmb_codegen_test: tests/CMakeFiles/libmb_codegen_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ego/projects/codegen/cmake-build-debug-clang/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable libmb_codegen_test"
	cd /home/ego/projects/codegen/cmake-build-debug-clang/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/libmb_codegen_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/libmb_codegen_test.dir/build: tests/libmb_codegen_test
.PHONY : tests/CMakeFiles/libmb_codegen_test.dir/build

tests/CMakeFiles/libmb_codegen_test.dir/clean:
	cd /home/ego/projects/codegen/cmake-build-debug-clang/tests && $(CMAKE_COMMAND) -P CMakeFiles/libmb_codegen_test.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/libmb_codegen_test.dir/clean

tests/CMakeFiles/libmb_codegen_test.dir/depend:
	cd /home/ego/projects/codegen/cmake-build-debug-clang && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ego/projects/codegen /home/ego/projects/codegen/tests /home/ego/projects/codegen/cmake-build-debug-clang /home/ego/projects/codegen/cmake-build-debug-clang/tests /home/ego/projects/codegen/cmake-build-debug-clang/tests/CMakeFiles/libmb_codegen_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/libmb_codegen_test.dir/depend

