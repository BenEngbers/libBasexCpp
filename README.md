2023-05-23

The current CMakeLists.txt is based on CMake 3.25
Functionality as provided by CMake 3.26 has not yet been implemented.

2024-07-14

libBasexCpp 1.0.0

CMakeLists.txt is based on Cmake 3.25.1.

The previous version of the source code was largely revised.
CmakeLists.txt now produces a shared library.

##Building the library
Instructions for creating and installing the library.
cd <path>/libBasexCpp
mkdir Build
cd Build
cmake .
cmake --build .
cmake --install . --prefix <destination>

## Building libBasexTest.cpp
cd ..
g++ -I ./include/ -c libBasexTest.cpp
g++ -L ~/lib/lib64 -Wall -o libTest libBasexTest.o -lBasexCpp

## Running libTest
LD_LIBRARY_PATH=<destination>/lib64 ./libTest
