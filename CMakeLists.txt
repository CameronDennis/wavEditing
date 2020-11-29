cmake_minimum_required(VERSION 3.14)
project(prgm2 C)

set(CMAKE_C_STANDARD 11)

add_executable(prgm2 main.c audio.c)

set (CMAKE_C_FLAGS "-Wall -Werror")