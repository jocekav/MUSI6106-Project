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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.22.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.22.2/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT/build

# Include any dependencies generated for this target.
include Jucer2CMake/CMakeFiles/Jucer2CMake.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include Jucer2CMake/CMakeFiles/Jucer2CMake.dir/compiler_depend.make

# Include the progress variables for this target.
include Jucer2CMake/CMakeFiles/Jucer2CMake.dir/progress.make

# Include the compile flags for this target's objects.
include Jucer2CMake/CMakeFiles/Jucer2CMake.dir/flags.make

Jucer2CMake/CMakeFiles/Jucer2CMake.dir/src/main.cpp.o: Jucer2CMake/CMakeFiles/Jucer2CMake.dir/flags.make
Jucer2CMake/CMakeFiles/Jucer2CMake.dir/src/main.cpp.o: ../Jucer2CMake/src/main.cpp
Jucer2CMake/CMakeFiles/Jucer2CMake.dir/src/main.cpp.o: Jucer2CMake/CMakeFiles/Jucer2CMake.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Jucer2CMake/CMakeFiles/Jucer2CMake.dir/src/main.cpp.o"
	cd /Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT/build/Jucer2CMake && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT Jucer2CMake/CMakeFiles/Jucer2CMake.dir/src/main.cpp.o -MF CMakeFiles/Jucer2CMake.dir/src/main.cpp.o.d -o CMakeFiles/Jucer2CMake.dir/src/main.cpp.o -c /Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT/Jucer2CMake/src/main.cpp

Jucer2CMake/CMakeFiles/Jucer2CMake.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Jucer2CMake.dir/src/main.cpp.i"
	cd /Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT/build/Jucer2CMake && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT/Jucer2CMake/src/main.cpp > CMakeFiles/Jucer2CMake.dir/src/main.cpp.i

Jucer2CMake/CMakeFiles/Jucer2CMake.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Jucer2CMake.dir/src/main.cpp.s"
	cd /Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT/build/Jucer2CMake && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT/Jucer2CMake/src/main.cpp -o CMakeFiles/Jucer2CMake.dir/src/main.cpp.s

# Object files for target Jucer2CMake
Jucer2CMake_OBJECTS = \
"CMakeFiles/Jucer2CMake.dir/src/main.cpp.o"

# External object files for target Jucer2CMake
Jucer2CMake_EXTERNAL_OBJECTS =

Jucer2CMake/Jucer2CMake: Jucer2CMake/CMakeFiles/Jucer2CMake.dir/src/main.cpp.o
Jucer2CMake/Jucer2CMake: Jucer2CMake/CMakeFiles/Jucer2CMake.dir/build.make
Jucer2CMake/Jucer2CMake: Jucer2CMake/libJucer2CMake_JUCE.a
Jucer2CMake/Jucer2CMake: Jucer2CMake/CMakeFiles/Jucer2CMake.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Jucer2CMake"
	cd /Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT/build/Jucer2CMake && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Jucer2CMake.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Jucer2CMake/CMakeFiles/Jucer2CMake.dir/build: Jucer2CMake/Jucer2CMake
.PHONY : Jucer2CMake/CMakeFiles/Jucer2CMake.dir/build

Jucer2CMake/CMakeFiles/Jucer2CMake.dir/clean:
	cd /Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT/build/Jucer2CMake && $(CMAKE_COMMAND) -P CMakeFiles/Jucer2CMake.dir/cmake_clean.cmake
.PHONY : Jucer2CMake/CMakeFiles/Jucer2CMake.dir/clean

Jucer2CMake/CMakeFiles/Jucer2CMake.dir/depend:
	cd /Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT /Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT/Jucer2CMake /Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT/build /Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT/build/Jucer2CMake /Users/vedant/Desktop/Programming/MUSI6106-Project/plugin_framework/FRUT/build/Jucer2CMake/CMakeFiles/Jucer2CMake.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Jucer2CMake/CMakeFiles/Jucer2CMake.dir/depend
