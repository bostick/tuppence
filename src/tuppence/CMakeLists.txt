project(tuppence)

find_package(LLVM REQUIRED CONFIG)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs
  Core
  ExecutionEngine
  RuntimeDyld
  native)
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  ../TuppenceConfig.h.in
  "${CMAKE_CURRENT_BINARY_DIR}/TuppenceConfig.h"
  )

# add the binary tree to the search path for include files
# so that we will find TuppenceConfig.h
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# add the interpreter
add_executable(tuppence
  Eval.h
  Eval.cpp
  FiniteWord.h
  FiniteWord.cpp
  Lexer.h
  Lexer.cpp
  Logger.h
  Logger.cpp
  Main.cpp
  Parser.h
  Parser.cpp
  RationalWord.h
  RationalWord.cpp
  Value.h
  Value.cpp
  ValueList.h
  ValueList.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/TuppenceConfig.h)

# Link against LLVM libraries
target_link_libraries(tuppence ${llvm_libs})

set_property(TARGET tuppence PROPERTY CXX_STANDARD 11)
set_property(TARGET tuppence PROPERTY CXX_STANDARD_REQUIRED ON)
