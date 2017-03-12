# This project is adapted from the actual LLVM sources, containing the bare minimum for providing the functionality for
# Tuppence (APInt, error handling, etc.), without making any changes to the source code itself.

# It is intended to be able to drop in new versions of the LLVM source as needed (or until the same functionality is provided elsewhere)


#brenton@DESKTOP-9DJHPDT:/mnt/c/dev/llvm/llvm$ svn info
#Path: .
#Working Copy Root Path: /mnt/c/dev/llvm/llvm
#URL: http://llvm.org/svn/llvm-project/llvm/trunk
#Relative URL: ^/llvm/trunk
#Repository Root: http://llvm.org/svn/llvm-project
#Repository UUID: 91177308-0d34-0410-b5e6-96231b3b80d8
#Revision: 288581
#Node Kind: directory
#Schedule: normal
#Last Changed Author: ctopper
#Last Changed Rev: 288578
#Last Changed Date: 2016-12-03 00:35:44 -0500 (Sat, 03 Dec 2016)

project(llvm-minimal)

# line 22
if(NOT DEFINED LLVM_VERSION_MAJOR)
  set(LLVM_VERSION_MAJOR 4)
endif()
if(NOT DEFINED LLVM_VERSION_MINOR)
  set(LLVM_VERSION_MINOR 0)
endif()
if(NOT DEFINED LLVM_VERSION_PATCH)
  set(LLVM_VERSION_PATCH 0)
endif()
if(NOT DEFINED LLVM_VERSION_SUFFIX)
  set(LLVM_VERSION_SUFFIX svn)
endif()

# line 42
if (NOT PACKAGE_VERSION)
  set(PACKAGE_VERSION
    "${LLVM_VERSION_MAJOR}.${LLVM_VERSION_MINOR}.${LLVM_VERSION_PATCH}${LLVM_VERSION_SUFFIX}")
endif()

# line 184
# Add path for custom modules
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
  )
  
# line 213
set(PACKAGE_NAME LLVM)

# line 302
set(LLVM_MAIN_SRC_DIR     ${CMAKE_CURRENT_SOURCE_DIR}  ) # --src-root

# line 355
set(LLVM_TARGET_ARCH "host"
  CACHE STRING "Set target to use for LLVM JIT or use \"host\" for automatic detection.")

# line 563
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

# line 710
# Configure the three LLVM configuration header files.
configure_file(
  include/llvm/Config/config.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/llvm/Config/config.h)
configure_file(
  include/llvm/Config/llvm-config.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/llvm/Config/llvm-config.h)
configure_file(
  include/llvm/Config/abi-breaking.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/llvm/Config/abi-breaking.h)
configure_file(
  include/llvm/Support/DataTypes.h.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/llvm/Support/DataTypes.h)
  
# find the generated LLVM headers
include_directories("${CMAKE_CURRENT_BINARY_DIR}/")

include_directories("include")

add_library(llvm-minimal
  ${CMAKE_CURRENT_BINARY_DIR}/llvm/Config/config.h
  
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
  lib/Support/Atomic.cpp
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
