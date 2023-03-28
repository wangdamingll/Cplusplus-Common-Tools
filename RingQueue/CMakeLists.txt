project(test)

cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 11)

aux_source_directory(. FILES)

add_executable(test ${FILES})