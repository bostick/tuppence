
project(tuppence-exe)

find_package(LLVM REQUIRED CONFIG)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  ../TuppenceConfig.h.in
  "${CMAKE_CURRENT_BINARY_DIR}/TuppenceConfig.h"
  )

# add the binary tree to the search path for include files
# so that we will find TuppenceConfig.h
include_directories("${CMAKE_CURRENT_BINARY_DIR}/")

# include tuppence/ dir
include_directories("..")


include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(tuppence-exe
  Main.cpp
  #runtime-listing.cpp
)

target_link_libraries(tuppence-exe tuppence-lib )

set_property(TARGET tuppence-exe PROPERTY CXX_STANDARD 11)
set_property(TARGET tuppence-exe PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-fno-rtti")

set_target_properties(tuppence-exe
        PROPERTIES OUTPUT_NAME tuppence)
