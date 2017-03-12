project(runUnitTests)

# find the generated LLVM headers
include_directories("${CMAKE_CURRENT_BINARY_DIR}/../llvm-minimal")

enable_testing()

include_directories("../llvm-minimal/include")
include_directories("../tuppence/")

# Add test cpp file
add_executable(runUnitTests
	Eval.test.cpp
	FiniteWord.test.cpp
	Parser.test.cpp
	RationalWord.test.cpp
	Value.test.cpp
)

# Link test executable against gtest & gtest_main
target_link_libraries(runUnitTests tuppencelib gtest gtest_main)

add_test(
    NAME runUnitTests
    COMMAND runUnitTests
)

set_property(TARGET runUnitTests PROPERTY CXX_STANDARD 11)
set_property(TARGET runUnitTests PROPERTY CXX_STANDARD_REQUIRED ON)
