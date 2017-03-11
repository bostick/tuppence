cmake_minimum_required (VERSION 2.8.8)

project(Tuppence)

# The version number.
set(Tuppence_VERSION_MAJOR 0)
set(Tuppence_VERSION_MINOR 1)
set(Tuppence_LOOP_LIMIT 1000)

# for making sure that gtest and gtest_main are copied into the same build dir as tuppence and runUnitTests
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#set(CMAKE_MACOSX_RPATH ON)

add_subdirectory(llvm)
add_subdirectory(googletest)
add_subdirectory(tuppence)
add_subdirectory(test)
