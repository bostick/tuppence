
project(tuppence-lib)

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


include_directories("../common")
include_directories(".")


include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# add the interpreter
add_library(tuppence-lib
  ../common/AST.h
  AST.cpp
  Eval.h
  Eval.cpp
  Eval2.cpp
  #FiniteWord.h
  #FiniteWord.cpp
  Interpreter.h
  Interpreter.cpp
  KaleidoscopeJIT.h
  ../common/Lexer.h
  Lexer.cpp
  #List.h
  #List.cpp
  #Logger.h
  #Logger.cpp
  #Method.h
  #Method.cpp
  Parser.h
  Parser.cpp
  #RationalWord.h
  #RationalWord.cpp
  #Value.h
  #Value.cpp
  
  #../runtime/Runtime.h
  #../runtime/Runtime.cpp
  #../runtime/FiniteWordImpl.h
  #../runtime/FiniteWordImpl.cpp
  #../runtime/FiniteWord.h
  #../runtime/FiniteWord.cpp
  )

# Find the libraries that correspond to the LLVM components
# that we wish to use
#llvm_map_components_to_libnames(llvm_libs
#    core
#ExecutionEngine
#RuntimeDyld
#    orcjit
#    native
#)

#ADD_LIBRARY(runtime STATIC IMPORTED)
#SET_TARGET_PROPERTIES(runtime PROPERTIES
#    IMPORTED_LOCATION "/Users/brenton/Documents/GitHub/tuppence/src/runtime/libruntime.a")

#target_link_libraries(tuppence finiteword ${llvm_libs})
#target_link_libraries(tuppence-lib ${llvm_libs})
target_link_libraries(tuppence-lib runtime)

set_property(TARGET tuppence-lib PROPERTY CXX_STANDARD 11)
set_property(TARGET tuppence-lib PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-fno-rtti")

set_target_properties(tuppence-lib
        PROPERTIES OUTPUT_NAME tuppence)

