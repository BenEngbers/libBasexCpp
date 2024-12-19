# Specify the minimum version.
  cmake_minimum_required(VERSION 3.25.1)

# Specify the project info.
  project(libBasexCpp VERSION 2.1.0 DESCRIPTION "LibBasexCpp  library")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wcast-align -Wcast-qual -Wnull-dereference -Woverloaded-virtual -Wformat=2 -Werror")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OBJCXX_STANDARD_REQUIRED ON)

# Add debugging option
# option(USE_SPACE "Enter debug mode" ON)
# if (USE_SPACE)
#   add_definitions(-DSPACE)
# endif()

include(FindPkgConfig)
pkg_check_modules(CURLPP REQUIRED curlpp)

# Declare the library target.
file (GLOB_RECURSE SOURCES src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SOURCES})
set_property(TARGET ${PROJECT_NAME} 
						 PROPERTY POSITION_INDEPENDENT_CODE ON
						 )

######## CURL STUFF ########
include(FindCURL)
find_package(CURL REQUIRED)
if(CURL_FOUND)
  message(STATUS "Found CURL version: ${CURL_VERSION_STRING}")
  message(STATUS "Using CURL include dir(s): ${CURL_INCLUDE_DIRS}")
  message(STATUS "Using CURL lib(s): ${CURL_LIBRARIES}")
else()
  message(FATAL_ERROR "Could not find CURL")
endif()

target_link_libraries(${PROJECT_NAME} ${CURLPP_LDFLAGS})
target_include_directories(${PROJECT_NAME} PRIVATE src)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

# Set the shared object version property to the project's major version.
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
# Set the public header property to the one with the actual API.
# set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER   src/BasexClient.h)

# prevent 'lib' prefix
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES PREFIX "")
# SET_TARGET_PROPERTIES(libBasexCpp PROPERTIES PREFIX "")

####### Install #########
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
