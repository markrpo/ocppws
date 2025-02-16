cmake_minimum_required(VERSION 3.10)
project(ocppws)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find libwebsockets
find_package(PkgConfig REQUIRED)
pkg_check_modules(LWS REQUIRED libwebsockets)

# Another way to find libwebsockets without pkg-config:
# Find libwebsockets include directory
# find_path(LIBWEBSOCKETS_INCLUDE_DIR libwebsockets.h)

# Find libwebsockets library
# find_library(LIBWEBSOCKETS_LIBRARIES NAMES websockets)

# Set sources and headers files
set(SOURCES src/ocppws.cpp)
set(HEADERS)

# Add executable
add_executable(ocppws ${SOURCES} ${HEADERS})

# Include directories PRIVATE means only this target can see the include directories
target_include_directories(ocppws PRIVATE include ${LWS_INCLUDE_DIRS}) 

# Link libraries
target_link_libraries(ocppws ${LWS_LIBRARIES})
