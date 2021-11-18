cmake_minimum_required(VERSION 3.10)
project(test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Debug")

add_executable(test main.cpp TimerMgr.h TimerMgr.cpp)

#valgrind发现的错误更精确，如能够定位到源代码行
list(APPEND flags "-g" "-O0")

target_compile_options(test
  PRIVATE
    ${flags}
  )