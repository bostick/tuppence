project(runUnitTests)

find_package(LLVM REQUIRED CONFIG)
find_package(GTest REQUIRED)

enable_testing()

# include tuppence/ dir
include_directories("..")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Add test cpp file
add_executable(runUnitTests
	Eval.test.cpp
	FiniteWord.test.cpp
	Parser.test.cpp
	RationalWord.test.cpp
	Value.test.cpp
)

# Link test executable against gtest & gtest_main
target_link_libraries(runUnitTests tuppence GTest::GTest GTest::Main)

add_test(
    NAME runUnitTests
    COMMAND runUnitTests
)

set_property(TARGET runUnitTests PROPERTY CXX_STANDARD 11)
set_property(TARGET runUnitTests PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-fno-rtti")
