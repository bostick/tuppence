
cmake_minimum_required (VERSION 3.0)

# https://youtu.be/eC9-iRN2b04?t=1454
if (MSVC)
	add_compile_options(/W3 /WX)
else()
	add_compile_options(-W -Wall -Werror -Wno-unused-parameter)
endif()


project(Tuppence)

# The version number.
set(Tuppence_VERSION_MAJOR 0)
set(Tuppence_VERSION_MINOR 2)
set(Tuppence_LOOP_LIMIT 1000)
  
#add_subdirectory(finiteword)
add_subdirectory(runtime)
add_subdirectory(lib)
add_subdirectory(exe)
#add_subdirectory(test)
