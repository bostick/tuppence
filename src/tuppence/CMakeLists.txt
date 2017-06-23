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
include_directories("${CMAKE_CURRENT_BINARY_DIR}/../llvm-minimal/include")

include_directories("../llvm-minimal/include")

# add the interpreter
add_library(tuppencelib
  Eval.h
  Eval.cpp
  FiniteWord.h
  FiniteWord.cpp
  Interpreter.h
  Interpreter.cpp
  Lexer.h
  Lexer.cpp
  Logger.h
  Logger.cpp
  Parser.h
  Parser.cpp
  RationalWord.h
  RationalWord.cpp
  Value.h
  Value.cpp
  ValueList.h
  ValueList.cpp)

add_executable(tuppence
  Main.cpp)

target_link_libraries(tuppencelib llvm-minimal)

set_property(TARGET tuppencelib PROPERTY CXX_STANDARD 11)
set_property(TARGET tuppencelib PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(tuppence tuppencelib)

set_property(TARGET tuppence PROPERTY CXX_STANDARD 11)
set_property(TARGET tuppence PROPERTY CXX_STANDARD_REQUIRED ON)
