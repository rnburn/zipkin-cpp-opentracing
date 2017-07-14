cmake_minimum_required(VERSION 3.0)
enable_testing()

project(zipkin-opentracing)
set(CMAKE_CXX_STANDARD 11)

find_package(CURL)
include_directories(SYSTEM ${CURL_INCLUDE_DIRS})


include_directories(SYSTEM 3rd_party/include)

include_directories(zipkin/include)
add_subdirectory(zipkin)

include_directories(zipkin_opentracing/include)
add_subdirectory(zipkin_opentracing)
