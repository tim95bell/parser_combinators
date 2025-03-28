
cmake_minimum_required(VERSION 3.25)

project(pc_tests VERSION 0.0.1 LANGUAGES CXX)

enable_testing()

add_executable("${PROJECT_NAME}"
    parsers_tests.cpp)

target_link_libraries("${PROJECT_NAME}" PRIVATE Catch2::Catch2WithMain parser_combinators)
