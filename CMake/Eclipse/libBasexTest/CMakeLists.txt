cmake_minimum_required (VERSION 3.25.1)
project (libBasexTest VERSION 1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wcast-align -Wcast-qual -Wnull-dereference -Woverloaded-virtual -Wformat=2 -Werror" )
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)

file (GLOB_RECURSE SOURCES src/libBasexTest.cpp)
add_executable(libBasexTest ${SOURCES})

target_include_directories(libBasexTest 
	PUBLIC "${PROJECT_BINARY_DIR}"
	)

target_link_libraries(${PROJECT_NAME} PUBLIC libBasexCpp)
message(STATUS "Dir 1 : ${PROJECT_NAME} PUBLIC subproject/libBasexCpp subproject/libBasexSocket")

add_subdirectory("/home/bengbers/Thuis/C_CPP_Werkbank/libBasexCpp"    subproject/libBasexCpp)
add_library(BasexCpp SHARED IMPORTED)						

target_include_directories(${PROJECT_NAME} PUBLIC  
		"${CMAKE_CURRENT_SOURCE_DIR}/include"
		)

