cmake_minimum_required(VERSION 3.12)
project(fact_pattern C)

set(CMAKE_C_STANDARD 11)

add_executable(fact_pattern
        src/library.c src/library.h)
