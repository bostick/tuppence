cmake_minimum_required (VERSION 2.8.8)

project(Tuppence)

# The version number.
set(Tuppence_VERSION_MAJOR 0)
set(Tuppence_VERSION_MINOR 1)
set(Tuppence_LOOP_LIMIT 1000)

add_subdirectory(llvm-minimal)
add_subdirectory(googletest)
add_subdirectory(tuppence)
add_subdirectory(test)
