aux_source_directory(. FILES)

FIND_PACKAGE(Threads REQUIRED)
if(NOT Threads_FOUND)
    message(FATAL_ERROR "not find thread")
endif()

add_executable(curltest ${FILES})

target_link_libraries(curltest 
  curl 
  Threads::Threads
)

target_compile_options(curltest
    PRIVATE
      ${CXX_FLAGS}
      ${THREADS_PREFER_PTHREAD_FLAG}
)

install(
  TARGETS
    curltest
  RUNTIME
    DESTINATION ${CMAKE_SOURCE_DIR}/bin
  )