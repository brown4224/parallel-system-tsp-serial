# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.8

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
CMAKE_COMMAND = /home/sean/Desktop/clion/bin/cmake/bin/cmake

# The command to remove a file.
RM = /home/sean/Desktop/clion/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/tsp.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/tsp.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tsp.dir/flags.make

CMakeFiles/tsp.dir/main.cpp.o: CMakeFiles/tsp.dir/flags.make
CMakeFiles/tsp.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/tsp.dir/main.cpp.o"
	mpic++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tsp.dir/main.cpp.o -c /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/main.cpp

CMakeFiles/tsp.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tsp.dir/main.cpp.i"
	mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/main.cpp > CMakeFiles/tsp.dir/main.cpp.i

CMakeFiles/tsp.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tsp.dir/main.cpp.s"
	mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/main.cpp -o CMakeFiles/tsp.dir/main.cpp.s

CMakeFiles/tsp.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/tsp.dir/main.cpp.o.requires

CMakeFiles/tsp.dir/main.cpp.o.provides: CMakeFiles/tsp.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/tsp.dir/build.make CMakeFiles/tsp.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/tsp.dir/main.cpp.o.provides

CMakeFiles/tsp.dir/main.cpp.o.provides.build: CMakeFiles/tsp.dir/main.cpp.o


CMakeFiles/tsp.dir/stack.cpp.o: CMakeFiles/tsp.dir/flags.make
CMakeFiles/tsp.dir/stack.cpp.o: ../stack.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/tsp.dir/stack.cpp.o"
	mpic++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tsp.dir/stack.cpp.o -c /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/stack.cpp

CMakeFiles/tsp.dir/stack.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tsp.dir/stack.cpp.i"
	mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/stack.cpp > CMakeFiles/tsp.dir/stack.cpp.i

CMakeFiles/tsp.dir/stack.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tsp.dir/stack.cpp.s"
	mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/stack.cpp -o CMakeFiles/tsp.dir/stack.cpp.s

CMakeFiles/tsp.dir/stack.cpp.o.requires:

.PHONY : CMakeFiles/tsp.dir/stack.cpp.o.requires

CMakeFiles/tsp.dir/stack.cpp.o.provides: CMakeFiles/tsp.dir/stack.cpp.o.requires
	$(MAKE) -f CMakeFiles/tsp.dir/build.make CMakeFiles/tsp.dir/stack.cpp.o.provides.build
.PHONY : CMakeFiles/tsp.dir/stack.cpp.o.provides

CMakeFiles/tsp.dir/stack.cpp.o.provides.build: CMakeFiles/tsp.dir/stack.cpp.o


CMakeFiles/tsp.dir/graphs.cpp.o: CMakeFiles/tsp.dir/flags.make
CMakeFiles/tsp.dir/graphs.cpp.o: ../graphs.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/tsp.dir/graphs.cpp.o"
	mpic++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tsp.dir/graphs.cpp.o -c /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/graphs.cpp

CMakeFiles/tsp.dir/graphs.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tsp.dir/graphs.cpp.i"
	mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/graphs.cpp > CMakeFiles/tsp.dir/graphs.cpp.i

CMakeFiles/tsp.dir/graphs.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tsp.dir/graphs.cpp.s"
	mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/graphs.cpp -o CMakeFiles/tsp.dir/graphs.cpp.s

CMakeFiles/tsp.dir/graphs.cpp.o.requires:

.PHONY : CMakeFiles/tsp.dir/graphs.cpp.o.requires

CMakeFiles/tsp.dir/graphs.cpp.o.provides: CMakeFiles/tsp.dir/graphs.cpp.o.requires
	$(MAKE) -f CMakeFiles/tsp.dir/build.make CMakeFiles/tsp.dir/graphs.cpp.o.provides.build
.PHONY : CMakeFiles/tsp.dir/graphs.cpp.o.provides

CMakeFiles/tsp.dir/graphs.cpp.o.provides.build: CMakeFiles/tsp.dir/graphs.cpp.o


CMakeFiles/tsp.dir/mpi_tsp.cpp.o: CMakeFiles/tsp.dir/flags.make
CMakeFiles/tsp.dir/mpi_tsp.cpp.o: ../mpi_tsp.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/tsp.dir/mpi_tsp.cpp.o"
	mpic++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/tsp.dir/mpi_tsp.cpp.o -c /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/mpi_tsp.cpp

CMakeFiles/tsp.dir/mpi_tsp.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tsp.dir/mpi_tsp.cpp.i"
	mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/mpi_tsp.cpp > CMakeFiles/tsp.dir/mpi_tsp.cpp.i

CMakeFiles/tsp.dir/mpi_tsp.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tsp.dir/mpi_tsp.cpp.s"
	mpic++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/mpi_tsp.cpp -o CMakeFiles/tsp.dir/mpi_tsp.cpp.s

CMakeFiles/tsp.dir/mpi_tsp.cpp.o.requires:

.PHONY : CMakeFiles/tsp.dir/mpi_tsp.cpp.o.requires

CMakeFiles/tsp.dir/mpi_tsp.cpp.o.provides: CMakeFiles/tsp.dir/mpi_tsp.cpp.o.requires
	$(MAKE) -f CMakeFiles/tsp.dir/build.make CMakeFiles/tsp.dir/mpi_tsp.cpp.o.provides.build
.PHONY : CMakeFiles/tsp.dir/mpi_tsp.cpp.o.provides

CMakeFiles/tsp.dir/mpi_tsp.cpp.o.provides.build: CMakeFiles/tsp.dir/mpi_tsp.cpp.o


# Object files for target tsp
tsp_OBJECTS = \
"CMakeFiles/tsp.dir/main.cpp.o" \
"CMakeFiles/tsp.dir/stack.cpp.o" \
"CMakeFiles/tsp.dir/graphs.cpp.o" \
"CMakeFiles/tsp.dir/mpi_tsp.cpp.o"

# External object files for target tsp
tsp_EXTERNAL_OBJECTS =

tsp: CMakeFiles/tsp.dir/main.cpp.o
tsp: CMakeFiles/tsp.dir/stack.cpp.o
tsp: CMakeFiles/tsp.dir/graphs.cpp.o
tsp: CMakeFiles/tsp.dir/mpi_tsp.cpp.o
tsp: CMakeFiles/tsp.dir/build.make
tsp: CMakeFiles/tsp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable tsp"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tsp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tsp.dir/build: tsp

.PHONY : CMakeFiles/tsp.dir/build

CMakeFiles/tsp.dir/requires: CMakeFiles/tsp.dir/main.cpp.o.requires
CMakeFiles/tsp.dir/requires: CMakeFiles/tsp.dir/stack.cpp.o.requires
CMakeFiles/tsp.dir/requires: CMakeFiles/tsp.dir/graphs.cpp.o.requires
CMakeFiles/tsp.dir/requires: CMakeFiles/tsp.dir/mpi_tsp.cpp.o.requires

.PHONY : CMakeFiles/tsp.dir/requires

CMakeFiles/tsp.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tsp.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tsp.dir/clean

CMakeFiles/tsp.dir/depend:
	cd /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/cmake-build-debug /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/cmake-build-debug /home/sean/Desktop/school/parralel-systems/tsp-finalized/parallel-system-tsp-serial/cmake-build-debug/CMakeFiles/tsp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tsp.dir/depend

