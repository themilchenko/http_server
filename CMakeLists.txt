cmake_minimum_required(VERSION 3.10)

project(http_server LANGUAGES C)

# Set default settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set debug flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
add_compile_definitions(DEBUG)

# Define sources and headers here
set(SOURCES src/utils.c src/config.c src/main.c src/server.c)
set(HEADERS include/utils.h include/config.h include/server.h)

# enabling birectory with header files
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add an executable target for your project
add_executable(http_server ${SOURCES} ${HEADERS})
