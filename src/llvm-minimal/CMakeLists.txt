# This project is adapted from the actual LLVM sources, containing the bare minimum for providing the functionality for
# Tuppence (APInt, error handling, etc.), without making any changes to the source code itself.

# It is intended to be able to drop in new versions of the LLVM source as needed (or until the same functionality is provided elsewhere)


#Brentons-MacBook-Pro:llvm brenton$ svn info
#Path: .
#Working Copy Root Path: /Users/brenton/llvm-4.0.1/llvm
#URL: http://llvm.org/svn/llvm-project/llvm/tags/RELEASE_401/final
#Relative URL: ^/llvm/tags/RELEASE_401/final
#Repository Root: http://llvm.org/svn/llvm-project
#Repository UUID: 91177308-0d34-0410-b5e6-96231b3b80d8
#Revision: 306117
#Node Kind: directory
#Schedule: normal
#Last Changed Author: tstellar
#Last Changed Rev: 305825
#Last Changed Date: 2017-06-20 15:29:32 -0400 (Tue, 20 Jun 2017)

project(llvm-minimal)

# line 22
if(NOT DEFINED LLVM_VERSION_MAJOR)
  set(LLVM_VERSION_MAJOR 4)
endif()
if(NOT DEFINED LLVM_VERSION_MINOR)
  set(LLVM_VERSION_MINOR 0)
endif()
if(NOT DEFINED LLVM_VERSION_PATCH)
  set(LLVM_VERSION_PATCH 1)
endif()
if(NOT DEFINED LLVM_VERSION_SUFFIX)
  set(LLVM_VERSION_SUFFIX "")
endif()

# line 42
if (NOT PACKAGE_VERSION)
  set(PACKAGE_VERSION
    "${LLVM_VERSION_MAJOR}.${LLVM_VERSION_MINOR}.${LLVM_VERSION_PATCH}${LLVM_VERSION_SUFFIX}")
endif()

# line 197
# Add path for custom modules
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
  )

# line 226
set(PACKAGE_NAME LLVM)
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_BUGREPORT "http://llvm.org/bugs/")

# line 312
# Each of them corresponds to llvm-config's.
set(LLVM_TOOLS_BINARY_DIR ${LLVM_RUNTIME_OUTPUT_INTDIR}) # --bindir
set(LLVM_LIBRARY_DIR      ${LLVM_LIBRARY_OUTPUT_INTDIR}) # --libdir
set(LLVM_MAIN_SRC_DIR     ${CMAKE_CURRENT_SOURCE_DIR}  ) # --src-root
set(LLVM_MAIN_INCLUDE_DIR ${LLVM_MAIN_SRC_DIR}/include ) # --includedir
set(LLVM_BINARY_DIR       ${CMAKE_CURRENT_BINARY_DIR}  ) # --prefix

# Note: LLVM_CMAKE_PATH does not include generated files
set(LLVM_CMAKE_PATH ${LLVM_MAIN_SRC_DIR}/cmake/modules)
set(LLVM_EXAMPLES_BINARY_DIR ${LLVM_BINARY_DIR}/examples)
set(LLVM_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)

# line 368
set(LLVM_TARGET_ARCH "host"
  CACHE STRING "Set target to use for LLVM JIT or use \"host\" for automatic detection.")

# line 388
include(AddLLVMDefinitions)

# line 579
# All options referred to from HandleLLVMOptions have to be specified
# BEFORE this include, otherwise options will not be correctly set on
# first cmake run
include(config-ix)

string(REPLACE "Native" ${LLVM_NATIVE_ARCH}
  LLVM_TARGETS_TO_BUILD "${LLVM_TARGETS_TO_BUILD}")
list(REMOVE_DUPLICATES LLVM_TARGETS_TO_BUILD)

# By default, we target the host, but this can be overridden at CMake
# invocation time.
set(LLVM_DEFAULT_TARGET_TRIPLE "${LLVM_HOST_TRIPLE}" CACHE STRING
  "Default target for which LLVM will generate code." )
set(TARGET_TRIPLE "${LLVM_DEFAULT_TARGET_TRIPLE}")
message(STATUS "LLVM host triple: ${LLVM_HOST_TRIPLE}")
message(STATUS "LLVM default target triple: ${LLVM_DEFAULT_TARGET_TRIPLE}")

include(HandleLLVMOptions)

# line 726
# Configure the three LLVM configuration header files.
configure_file(
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Config/config.h.cmake
  ${LLVM_INCLUDE_DIR}/llvm/Config/config.h)
configure_file(
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Config/llvm-config.h.cmake
  ${LLVM_INCLUDE_DIR}/llvm/Config/llvm-config.h)
configure_file(
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Config/abi-breaking.h.cmake
  ${LLVM_INCLUDE_DIR}/llvm/Config/abi-breaking.h)
configure_file(
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Support/DataTypes.h.cmake
  ${LLVM_INCLUDE_DIR}/llvm/Support/DataTypes.h)

#include_directories("${CMAKE_CURRENT_BINARY_DIR}/")

include_directories("include")

include_directories("${LLVM_INCLUDE_DIR}/")

add_library(llvm-minimal
  ${LLVM_INCLUDE_DIR}/llvm/Config/config.h
  ${LLVM_INCLUDE_DIR}/llvm/Config/llvm-config.h

  lib/Support/APInt.cpp
  lib/Support/FoldingSet.cpp
  lib/Support/Debug.cpp
  lib/Support/Errno.cpp
  lib/Support/Error.cpp
  lib/Support/ErrorHandling.cpp
  lib/Support/raw_ostream.cpp
  lib/Support/Hashing.cpp
  lib/Support/StringRef.cpp
  lib/Support/ManagedStatic.cpp
  lib/Support/CommandLine.cpp
  lib/Support/Signals.cpp
  lib/Support/circular_raw_ostream.cpp
  lib/Support/Mutex.cpp
  lib/Support/Twine.cpp
  lib/Support/Path.cpp
  lib/Support/NativeFormatting.cpp
  lib/Support/Process.cpp
  lib/Support/Threading.cpp
  lib/Support/StringMap.cpp
  lib/Support/Host.cpp
  lib/Support/Program.cpp
  lib/Support/SmallPtrSet.cpp
  lib/Support/ConvertUTFWrapper.cpp
  lib/Support/MemoryBuffer.cpp
  lib/Support/StringSaver.cpp
  lib/Support/StringExtras.cpp
  lib/Support/Triple.cpp
  lib/Support/ConvertUTF.cpp
  lib/Support/TargetParser.cpp
  lib/Support/SmallVector.cpp
  )

set_property(TARGET llvm-minimal PROPERTY CXX_STANDARD 11)
set_property(TARGET llvm-minimal PROPERTY CXX_STANDARD_REQUIRED ON)
