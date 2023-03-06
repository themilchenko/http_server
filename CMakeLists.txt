cmake_minimum_required(VERSION 3.10)

project("http_server" LANGUAGES C)

# Set default settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_definitions(DEBUG)

# Declarate all headers and its sources
set(
    ${PROJECT_NAME}_SERVER_LIB_HEADERS ${CMAKE_SOURCE_DIR}/include/server.h
    ${PROJECT_NAME}_CONFIG_LIB_HEADERS ${CMAKE_SOURCE_DIR}/include/config.h
)
set(
    ${PROJECT_NAME}_SERVER_LIB_SOURCES ${CMAKE_SOURCE_DIR}/src/server.c
    ${PROJECT_NAME}_CONFIG_LIB_SOURCES ${CMAKE_SOURCE_DIR}/src/config.c
)

set(${PROJECT_NAME}_CMD_SOURCES ${CMAKE_SOURCE_DIR}/src/main.c)

# Build project
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_CMD_SOURCES})
add_library(
    ${PROJECT_NAME}_SERVER_LIB STATIC
    ${${PROJECT_NAME}_SERVER_LIB_SOURCES}
)

target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_SERVER_LIB)

