cmake_minimum_required(VERSION 3.16)
project(test)

aux_source_directory(./ FILES)

set(CMAKE_CXX_STANDARD 11)

add_executable(test ${FILES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(test PRIVATE Threads::Threads)

