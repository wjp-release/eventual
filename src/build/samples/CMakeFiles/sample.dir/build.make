# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

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
CMAKE_COMMAND = /opt/local/bin/cmake

# The command to remove a file.
RM = /opt/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/wjp/Desktop/workspace/eventual/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/wjp/Desktop/workspace/eventual/src/build

# Include any dependencies generated for this target.
include samples/CMakeFiles/sample.dir/depend.make

# Include the progress variables for this target.
include samples/CMakeFiles/sample.dir/progress.make

# Include the compile flags for this target's objects.
include samples/CMakeFiles/sample.dir/flags.make

samples/CMakeFiles/sample.dir/sample.cc.o: samples/CMakeFiles/sample.dir/flags.make
samples/CMakeFiles/sample.dir/sample.cc.o: ../samples/sample.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/wjp/Desktop/workspace/eventual/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object samples/CMakeFiles/sample.dir/sample.cc.o"
	cd /Users/wjp/Desktop/workspace/eventual/src/build/samples && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sample.dir/sample.cc.o -c /Users/wjp/Desktop/workspace/eventual/src/samples/sample.cc

samples/CMakeFiles/sample.dir/sample.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sample.dir/sample.cc.i"
	cd /Users/wjp/Desktop/workspace/eventual/src/build/samples && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/wjp/Desktop/workspace/eventual/src/samples/sample.cc > CMakeFiles/sample.dir/sample.cc.i

samples/CMakeFiles/sample.dir/sample.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sample.dir/sample.cc.s"
	cd /Users/wjp/Desktop/workspace/eventual/src/build/samples && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/wjp/Desktop/workspace/eventual/src/samples/sample.cc -o CMakeFiles/sample.dir/sample.cc.s

# Object files for target sample
sample_OBJECTS = \
"CMakeFiles/sample.dir/sample.cc.o"

# External object files for target sample
sample_EXTERNAL_OBJECTS =

samples/sample: samples/CMakeFiles/sample.dir/sample.cc.o
samples/sample: samples/CMakeFiles/sample.dir/build.make
samples/sample: eventual/libeventual.a
samples/sample: samples/CMakeFiles/sample.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/wjp/Desktop/workspace/eventual/src/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable sample"
	cd /Users/wjp/Desktop/workspace/eventual/src/build/samples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sample.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
samples/CMakeFiles/sample.dir/build: samples/sample

.PHONY : samples/CMakeFiles/sample.dir/build

samples/CMakeFiles/sample.dir/clean:
	cd /Users/wjp/Desktop/workspace/eventual/src/build/samples && $(CMAKE_COMMAND) -P CMakeFiles/sample.dir/cmake_clean.cmake
.PHONY : samples/CMakeFiles/sample.dir/clean

samples/CMakeFiles/sample.dir/depend:
	cd /Users/wjp/Desktop/workspace/eventual/src/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/wjp/Desktop/workspace/eventual/src /Users/wjp/Desktop/workspace/eventual/src/samples /Users/wjp/Desktop/workspace/eventual/src/build /Users/wjp/Desktop/workspace/eventual/src/build/samples /Users/wjp/Desktop/workspace/eventual/src/build/samples/CMakeFiles/sample.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : samples/CMakeFiles/sample.dir/depend

