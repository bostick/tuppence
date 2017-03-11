project(tuppence)
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  ../TuppenceConfig.h.in
  "${CMAKE_CURRENT_BINARY_DIR}/TuppenceConfig.h"
  )


# add the binary tree to the search path for include files
# so that we will find TuppenceConfig.h
include_directories("${CMAKE_CURRENT_BINARY_DIR}/")

# find the generated LLVM headers
include_directories("${CMAKE_CURRENT_BINARY_DIR}/../llvm")

include_directories("../llvm/include")

# add the interpreter
add_library(tuppencelib
  Eval.cpp
  FiniteWord.cpp
  Lexer.cpp
  Logger.cpp
  Parser.cpp
  RationalWord.cpp
  Value.cpp
  ValueList.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/TuppenceConfig.h)

add_executable(tuppence
  Main.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/TuppenceConfig.h)

target_link_libraries(tuppencelib llvm-minimal)

target_link_libraries(tuppence tuppencelib)

set_property(TARGET tuppence PROPERTY CXX_STANDARD 11)
set_property(TARGET tuppence PROPERTY CXX_STANDARD_REQUIRED ON)
