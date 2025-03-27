
cmake_minimum_required(VERSION 3.25)

project(parser_combinators VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS FALSE)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(warnings
    -Wall
    -Wextra
    -Werror
    -Wshadow
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Wcast-align
    -Wunused
    -Woverloaded-virtual
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wnull-dereference
    -Wdouble-promotion
    -Wformat=2
    -Wimplicit-fallthrough)

add_library(parser_combinators STATIC
    src/pc.cpp)

target_include_directories(parser_combinators PUBLIC include)
target_compile_options(parser_combinators PRIVATE ${warnings})
