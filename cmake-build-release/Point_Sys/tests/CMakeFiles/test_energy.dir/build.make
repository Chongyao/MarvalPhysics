# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

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
CMAKE_COMMAND = /home/zcy/Downloads/clion-2018.3.2/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/zcy/Downloads/clion-2018.3.2/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/zcy/Documents/projects/MarvelPhysics

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zcy/Documents/projects/MarvelPhysics/cmake-build-release

# Include any dependencies generated for this target.
include Point_Sys/tests/CMakeFiles/test_energy.dir/depend.make

# Include the progress variables for this target.
include Point_Sys/tests/CMakeFiles/test_energy.dir/progress.make

# Include the compile flags for this target's objects.
include Point_Sys/tests/CMakeFiles/test_energy.dir/flags.make

Point_Sys/tests/CMakeFiles/test_energy.dir/test_energy.cc.o: Point_Sys/tests/CMakeFiles/test_energy.dir/flags.make
Point_Sys/tests/CMakeFiles/test_energy.dir/test_energy.cc.o: ../Point_Sys/tests/test_energy.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zcy/Documents/projects/MarvelPhysics/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Point_Sys/tests/CMakeFiles/test_energy.dir/test_energy.cc.o"
	cd /home/zcy/Documents/projects/MarvelPhysics/cmake-build-release/Point_Sys/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_energy.dir/test_energy.cc.o -c /home/zcy/Documents/projects/MarvelPhysics/Point_Sys/tests/test_energy.cc

Point_Sys/tests/CMakeFiles/test_energy.dir/test_energy.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_energy.dir/test_energy.cc.i"
	cd /home/zcy/Documents/projects/MarvelPhysics/cmake-build-release/Point_Sys/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/zcy/Documents/projects/MarvelPhysics/Point_Sys/tests/test_energy.cc > CMakeFiles/test_energy.dir/test_energy.cc.i

Point_Sys/tests/CMakeFiles/test_energy.dir/test_energy.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_energy.dir/test_energy.cc.s"
	cd /home/zcy/Documents/projects/MarvelPhysics/cmake-build-release/Point_Sys/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/zcy/Documents/projects/MarvelPhysics/Point_Sys/tests/test_energy.cc -o CMakeFiles/test_energy.dir/test_energy.cc.s

# Object files for target test_energy
test_energy_OBJECTS = \
"CMakeFiles/test_energy.dir/test_energy.cc.o"

# External object files for target test_energy
test_energy_EXTERNAL_OBJECTS =

bin/test_energy: Point_Sys/tests/CMakeFiles/test_energy.dir/test_energy.cc.o
bin/test_energy: Point_Sys/tests/CMakeFiles/test_energy.dir/build.make
bin/test_energy: Point_Sys/src/libPointSys.so
bin/test_energy: src/libmarvel-core.so
bin/test_energy: /usr/lib/x86_64-linux-gnu/libboost_filesystem.a
bin/test_energy: /usr/lib/x86_64-linux-gnu/libboost_system.a
bin/test_energy: /usr/lib/x86_64-linux-gnu/libboost_program_options.a
bin/test_energy: /usr/lib/x86_64-linux-gnu/libtbb.so
bin/test_energy: Point_Sys/tests/CMakeFiles/test_energy.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/zcy/Documents/projects/MarvelPhysics/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../../bin/test_energy"
	cd /home/zcy/Documents/projects/MarvelPhysics/cmake-build-release/Point_Sys/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_energy.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Point_Sys/tests/CMakeFiles/test_energy.dir/build: bin/test_energy

.PHONY : Point_Sys/tests/CMakeFiles/test_energy.dir/build

Point_Sys/tests/CMakeFiles/test_energy.dir/clean:
	cd /home/zcy/Documents/projects/MarvelPhysics/cmake-build-release/Point_Sys/tests && $(CMAKE_COMMAND) -P CMakeFiles/test_energy.dir/cmake_clean.cmake
.PHONY : Point_Sys/tests/CMakeFiles/test_energy.dir/clean

Point_Sys/tests/CMakeFiles/test_energy.dir/depend:
	cd /home/zcy/Documents/projects/MarvelPhysics/cmake-build-release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zcy/Documents/projects/MarvelPhysics /home/zcy/Documents/projects/MarvelPhysics/Point_Sys/tests /home/zcy/Documents/projects/MarvelPhysics/cmake-build-release /home/zcy/Documents/projects/MarvelPhysics/cmake-build-release/Point_Sys/tests /home/zcy/Documents/projects/MarvelPhysics/cmake-build-release/Point_Sys/tests/CMakeFiles/test_energy.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Point_Sys/tests/CMakeFiles/test_energy.dir/depend

