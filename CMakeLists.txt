cmake_minimum_required(VERSION 3.10)

project(myProject C)

add_executable(greeting src/main.c src/greeting.c)

target_include_directories(greeting PUBLIC include)
