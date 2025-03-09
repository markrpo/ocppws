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

# Find libwebsockets
find_package(PkgConfig REQUIRED)
pkg_check_modules(LWS REQUIRED libwebsockets)

find_path(NLOHMANN_INCLUDE_DIR
	NAMES json.hpp
	PATHS /usr/include/nlohmann /usr/local/include/nlohmann
	)

# Another way to find libwebsockets without pkg-config:
# Find libwebsockets include directory
# find_path(LIBWEBSOCKETS_INCLUDE_DIR libwebsockets.h)

# Find libwebsockets library
# find_library(LIBWEBSOCKETS_LIBRARIES NAMES websockets)

# Set sources and headers files
set(SOURCES src/main.cpp src/ocpp.cpp src/responses.cpp src/websocket.cpp)
set(HEADERS src/ocpp.hpp src/responses.hpp src/websocket.hpp src/iwebsocket.hpp)

# Add executable
add_executable(ocppws ${SOURCES} ${HEADERS})

# Include directories PRIVATE means only this target can see the include directories
target_include_directories(ocppws PRIVATE include ${LWS_INCLUDE_DIRS} ${NLOHMANN_INCLUDE_DIR})

# Link libraries
target_link_libraries(ocppws ${LWS_LIBRARIES})
