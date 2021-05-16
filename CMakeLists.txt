
cmake_minimum_required (VERSION 3.0)

if (MSVC) 
	add_compile_options(/W3 /WX)
else()
	add_compile_options(-W -Wall -Werror)
endif()

add_subdirectory(src)
