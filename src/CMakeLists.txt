# Find libwebsockets
find_package(PkgConfig REQUIRED)
pkg_check_modules(LWS REQUIRED libwebsockets)

find_path(NLOHMANN_INCLUDE_DIR
	NAMES json.hpp
	PATHS /usr/include/nlohmann /usr/local/include/nlohmann
	)

set(CMAKE_BUILD_TYPE Debug)

# Another way to find libwebsockets without pkg-config:
# Find libwebsockets include directory
# find_path(LIBWEBSOCKETS_INCLUDE_DIR libwebsockets.h)

# Find libwebsockets library
# find_library(LIBWEBSOCKETS_LIBRARIES NAMES websockets)

# Set sources and headers files
SET(LIB_SOURCES
    ocpp.cpp
    responses.cpp
    websocket.cpp
)

SET(LIB_HEADERS
    ocpp.hpp
    responses.hpp
    websocket.hpp
    iwebsocket.hpp
    iocpp.hpp
)

add_library(ocpplib SHARED ${LIB_SOURCES} ${LIB_HEADERS})
#add_library(ocpplib STATIC ${LIB_SOURCES} ${LIB_HEADERS})
target_include_directories(ocpplib PUBLIC include ${LWS_INCLUDE_DIRS} ${NLOHMANN_INCLUDE_DIR})
target_link_libraries(ocpplib ${LWS_LIBRARIES})
set_target_properties(ocpplib PROPERTIES CXX_STANDARD 17 POSITION_INDEPENDENT_CODE ON)

set(SOURCES main.cpp service.cpp)
set(HEADERS service.hpp)

# Add executable
add_executable(ocppws ${SOURCES} ${HEADERS})

# Include directories PRIVATE means only this target can see the include directories
target_include_directories(ocppws PRIVATE include)

# Link libraries
target_link_libraries(ocppws ocpplib)
