
# llc -filetype=obj runtime.ll
# ar r libruntime.a runtime.o

project(runtime)


add_definitions(-DRUNTIME_DLL)



find_package(LLVM REQUIRED CONFIG)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  ../TuppenceConfig.h.in
  "${CMAKE_CURRENT_BINARY_DIR}/config/TuppenceConfig.h"
  )

# add the binary tree to the search path for include files
# so that we will find TuppenceConfig.h
include_directories("${CMAKE_CURRENT_BINARY_DIR}/config/")

# include runtime/ dir
include_directories("..")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# add the interpreter
add_library(runtime SHARED
    ${CMAKE_CURRENT_BINARY_DIR}/config/TuppenceConfig.h
    ../common/Runtime.h
    Runtime.cpp
    #../common/AST.h
    ../common/Lexer.h
    ../common/TuppenceValue.h
    TuppenceValue.cpp
    #FiniteWordImpl.h
    #FiniteWordImpl.cpp
    ../common/FiniteWord.h
    FiniteWord.cpp
    ../common/Library.h
    Library.cpp
    ../common/List.h
    List.cpp
    ../common/RationalWord.h
    RationalWord.cpp
    ../common/TuppenceMath.h
    TuppenceMath.cpp
    ../common/Logger.h
    Logger.cpp

    ../common/BuiltinSymbol.h
    BuiltinSymbol.cpp

    ../common/BuiltinFunction.h
    BuiltinFunction.cpp
  )

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs
  core
  ExecutionEngine
  RuntimeDyld
  orcjit
  native
)

target_link_libraries(runtime ${llvm_libs})

set_property(TARGET runtime PROPERTY CXX_STANDARD 11)
set_property(TARGET runtime PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-fno-rtti")
