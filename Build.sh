#!/bin/bash

cur=`pwd`

for arg in "$@"
do
    case $arg in
        1 ) echo "### Compileer CMake/Eclipse/libBasexCpp"
            cd "$cur"/CMake/Eclipse/libBasexCpp
            pwd
            cmake -S . -B Build/Debug --fresh --preset=dev-linux  -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Debug -G Ninja
            cd Build/Debug
            pwd
            ninja
            ;;
        2 ) echo "### Compileer CMake/Eclipse/libBasexTest"
            cd "$cur"/CMake/Eclipse/libBasexTest
            pwd
            cmake -S . -B Build/Debug --fresh --preset=dev-linux  -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Debug -G Ninja
            cd Build/Debug
            pwd
            ninja
            echo "Use '"$cur"/CMake/Eclipse/libBasexTest/Build/Debug/libBasexTest' to execute Test"
            ;;
        3 ) echo "### Compileer CMake/BasexCpp"

            cd "$cur"/CMake/BasexCpp
            pwd
            # cd Build/Debug
            cmake -S . -B Build/Release --fresh --preset=dev-linux  -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Release -G Ninja
            cd Build/Release
            pwd
            ninja
            ;;
        4 ) echo "### Compileer Raw/Build/libBasexCpp library"
            mkdir -p -v ~/lib/lib64
            cd "$cur"/Raw/Build/libBasexCpp/src
            pwd

            # Build LD_LIBRARY
            g++ -c -fPIC -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wcast-align -Wcast-qual -Wnull-dereference -Woverloaded-virtual -Werror -I/usr/include/curl Base.cpp  BasexClient.cpp  BasexSocket.cpp  ClientUtils.cpp  QueryObject.cpp  ResponseObj.cpp -I/usr/include/curl
            echo "Compileer de shared Cpp library"
#            g++ -c -fPIC -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wcast-align -Wcast-qual -Wnull-dereference -Woverloaded-virtual -Werror -I/usr/include/curl Base.cpp  BasexClient.cpp  BasexSocket.cpp  ClientUtils.cpp  QueryObject.cpp  ResponseObj.cpp

            ## g++ -c -fPIC -std=c++20 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wcast-align -Wcast-qual -Wnull-dereference -Woverloaded-virtual -Wformat=2 -Werror -I/usr/include/curl Base.cpp  BasexClient.cpp  BasexSocket.cpp  ClientUtils.cpp  QueryObject.cpp  ResponseObj.cpp -I/usr/include/curl

            # -Wl,-soname,libBasexCpp.so
            g++ -shared -o ~/lib/lib64/libBasexCpp.so Base.o BasexClient.o  BasexSocket.o  ClientUtils.o  QueryObject.o  ResponseObj.o -L/usr/lib64 -lcurlpp

            # export LD_LIBRARY_PATH=/home/bengbers/lib/lib64:$LD_LIBRARY_PATH

            echo "Compileer de tester"
            cd "$cur"/Raw/Build/libBasexCpp
            echo "$PWD"
            g++ -I ./include -c libBasexTest.cpp
            g++ -L /usr/local/lib64 -lBasexCpp -Wall -o ./libTest libBasexTest.o -lBasexCpp
            echo "Use 'LD_LIBRARY_PATH=/home/bengbers/lib/lib64 "$cur"/Raw/Build/libBasexCpp/libTest' to execute ./libTest"
            echo "Use 'LD_PRELOAD=/home/bengbers/lib/lib64/libBasexCpp.so valgrind --keep-debuginfo=yes --leak-check=full  "$cur"/Raw/Build/libBasexCpp/libTest' for valgrind"
            ;;
        5 ) echo "### Compileer Raw/Use/libBasexCpp library"
            cd "$cur"/Raw/Use/libBasexCpp
            echo "$PWD"
            g++ -I ./include -c libBasexTest.cpp
            g++ -L/usr/local/lib64  -lBasexCpp -Wall -o ./libTest libBasexTest.o
            echo "Use 'LD_LIBRARY_PATH=/usr/local/lib64 "$cur"/Raw/Use/libBasexCpp/libTest' to execute ./libTest"
            echo "Use 'LD_PRELOAD=/use/local/lib64/libBasexCpp.so valgrind --keep-debuginfo=yes --leak-check=full  "$cur"/Raw/Build/libBasexCpp/libTest' for valgrind"
            ;;
    esac
done

