cmake_minimum_required(VERSION 3.10)
project(ocppws)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Check if clang is used and change to use libc++ instead of libstdc++
# To use clang use the following command in build directory:
# cmake -DCMAKE_CXX_COMPILER=clang++ ..
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
endif()

add_subdirectory(src)

# To execute tests, run the following command in build directory:
# cmake -DTEST=ON ..
if(TEST)
	add_subdirectory(test)
endif()
