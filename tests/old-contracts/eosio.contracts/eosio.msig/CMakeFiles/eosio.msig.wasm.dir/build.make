# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.15.5/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.15.5/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts/build

# Include any dependencies generated for this target.
include eosio.msig/CMakeFiles/eosio.msig.wasm.dir/depend.make

# Include the progress variables for this target.
include eosio.msig/CMakeFiles/eosio.msig.wasm.dir/progress.make

# Include the compile flags for this target's objects.
include eosio.msig/CMakeFiles/eosio.msig.wasm.dir/flags.make

eosio.msig/CMakeFiles/eosio.msig.wasm.dir/src/eosio.msig.cpp.o: eosio.msig/CMakeFiles/eosio.msig.wasm.dir/flags.make
eosio.msig/CMakeFiles/eosio.msig.wasm.dir/src/eosio.msig.cpp.o: ../eosio.msig/src/eosio.msig.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object eosio.msig/CMakeFiles/eosio.msig.wasm.dir/src/eosio.msig.cpp.o"
	cd /Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts/build/eosio.msig && /usr/local/eosio.cdt/bin/eosio-cpp  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/eosio.msig.wasm.dir/src/eosio.msig.cpp.o -c /Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts/eosio.msig/src/eosio.msig.cpp

eosio.msig/CMakeFiles/eosio.msig.wasm.dir/src/eosio.msig.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/eosio.msig.wasm.dir/src/eosio.msig.cpp.i"
	cd /Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts/build/eosio.msig && /usr/local/eosio.cdt/bin/eosio-cpp $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts/eosio.msig/src/eosio.msig.cpp > CMakeFiles/eosio.msig.wasm.dir/src/eosio.msig.cpp.i

eosio.msig/CMakeFiles/eosio.msig.wasm.dir/src/eosio.msig.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/eosio.msig.wasm.dir/src/eosio.msig.cpp.s"
	cd /Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts/build/eosio.msig && /usr/local/eosio.cdt/bin/eosio-cpp $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts/eosio.msig/src/eosio.msig.cpp -o CMakeFiles/eosio.msig.wasm.dir/src/eosio.msig.cpp.s

# Object files for target eosio.msig.wasm
eosio_msig_wasm_OBJECTS = \
"CMakeFiles/eosio.msig.wasm.dir/src/eosio.msig.cpp.o"

# External object files for target eosio.msig.wasm
eosio_msig_wasm_EXTERNAL_OBJECTS =

eosio.msig/eosio.msig.wasm: eosio.msig/CMakeFiles/eosio.msig.wasm.dir/src/eosio.msig.cpp.o
eosio.msig/eosio.msig.wasm: eosio.msig/CMakeFiles/eosio.msig.wasm.dir/build.make
eosio.msig/eosio.msig.wasm: eosio.msig/CMakeFiles/eosio.msig.wasm.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable eosio.msig.wasm"
	cd /Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts/build/eosio.msig && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/eosio.msig.wasm.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
eosio.msig/CMakeFiles/eosio.msig.wasm.dir/build: eosio.msig/eosio.msig.wasm

.PHONY : eosio.msig/CMakeFiles/eosio.msig.wasm.dir/build

eosio.msig/CMakeFiles/eosio.msig.wasm.dir/clean:
	cd /Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts/build/eosio.msig && $(CMAKE_COMMAND) -P CMakeFiles/eosio.msig.wasm.dir/cmake_clean.cmake
.PHONY : eosio.msig/CMakeFiles/eosio.msig.wasm.dir/clean

eosio.msig/CMakeFiles/eosio.msig.wasm.dir/depend:
	cd /Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts /Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts/eosio.msig /Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts/build /Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts/build/eosio.msig /Users/basarcancelebci/Work/API-market/Chain-Production/eosio.contracts/build/eosio.msig/CMakeFiles/eosio.msig.wasm.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : eosio.msig/CMakeFiles/eosio.msig.wasm.dir/depend

