cmake_minimum_required(VERSION 3.10)

project(http_server LANGUAGES C)

# Set default settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
add_compile_definitions(DEBUG)

# Define sources and headers here
set(SOURCES src/config.c src/main.c src/server.c)
set(HEADERS include/config.h include/server.h)

# enabling birectory with header files
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add an executable target for your project
add_executable(http_server ${SOURCES} ${HEADERS})

# # Declare all headers and its sources
# set(SERVER_LIB_HEADERS include/server.h)
# set(SERVER_LIB_SOURCES src/server.c)
# set(CONFIG_LIB_HEADERS include/config.h)
# set(CONFIG_LIB_SOURCES src/config.c)

# set(CMD_SOURCES src/main.c)

# # Build project
# add_executable(http_server ${CMD_SOURCES})
# add_library(server_lib STATIC ${SERVER_LIB_SOURCES})
# add_library(config_lib STATIC ${CONFIG_LIB_SOURCES})

# target_link_libraries(http_server PRIVATE server_lib)
# target_link_libraries(http_server PRIVATE config_lib)

# target_include_directories(http_server PRIVATE include)
# target_include_directories(server_lib PRIVATE include)
# target_include_directories(config_lib PRIVATE include)
