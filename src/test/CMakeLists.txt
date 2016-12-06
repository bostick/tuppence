project(runUnitTests)

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

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

include_directories("../tuppence/")

# Add test cpp file
add_executable(runUnitTests
	Eval.test.cpp
	FiniteWord.test.cpp
	Parser.test.cpp
	RationalWord.test.cpp
	Value.test.cpp

	../tuppence/Eval.h
	../tuppence/Eval.cpp
	../tuppence/FiniteWord.h
	../tuppence/FiniteWord.cpp
	../tuppence/Lexer.h
	../tuppence/Lexer.cpp
	../tuppence/Logger.h
	../tuppence/Logger.cpp
	../tuppence/Parser.h
	../tuppence/Parser.cpp
	../tuppence/RationalWord.h
	../tuppence/RationalWord.cpp
	../tuppence/Value.h
	../tuppence/Value.cpp
	../tuppence/ValueList.h
	../tuppence/ValueList.cpp
	${CMAKE_CURRENT_BINARY_DIR}/TuppenceConfig.h
)

# Link test executable against gtest & gtest_main
target_link_libraries(runUnitTests ${GTEST_BOTH_LIBRARIES})
target_link_libraries(runUnitTests ${llvm_libs})

add_test(
    NAME runUnitTests
    COMMAND runUnitTests
)

set_property(TARGET runUnitTests PROPERTY CXX_STANDARD 11)
set_property(TARGET runUnitTests PROPERTY CXX_STANDARD_REQUIRED ON)
