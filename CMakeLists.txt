cmake_minimum_required(VERSION 3.0)
enable_testing()

project(zipkin-opentracing)
set(CMAKE_CXX_STANDARD 11)

include_directories(SYSTEM 3rd_party/include)

include_directories(include)
add_library(zipkin src/zipkin_core_types.cc
                   src/utility.cc
                   src/hex.cc
                   src/tracer.cc
                   src/address.cc
                   src/span_buffer.cc
                   src/span_context.cc
                   src/opentracing.cc)

add_subdirectory(test)
