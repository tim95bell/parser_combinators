
cmake_minimum_required(VERSION 3.25)

project(pc_tests VERSION 0.0.1 LANGUAGES CXX)

set(parsers_tests parsers_tests)
set(combinators_tests combinators_tests)

add_executable("${parsers_tests}"
    parsers_tests.cpp)
target_link_libraries("${parsers_tests}" PRIVATE Catch2::Catch2WithMain parser_combinators)

add_executable("${combinators_tests}"
    combinators_tests.cpp)
target_link_libraries("${combinators_tests}" PRIVATE Catch2::Catch2WithMain parser_combinators)
