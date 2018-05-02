cmake_minimum_required(VERSION 3.0)

project(zipkin-opentracing)

set(ZIPKIN_VERSION_MAJOR "0")
set(ZIPKIN_VERSION_MINOR "3")
set(ZIPKIN_VERSION_PATCH "1")
set(ZIPKIN_VERSION_STRING
  "${ZIPKIN_VERSION_MAJOR}.${ZIPKIN_VERSION_MINOR}.${ZIPKIN_VERSION_PATCH}")

option(BUILD_SHARED_LIBS "Build as a shared library" ON)
option(BUILD_STATIC_LIBS "Build as a static library" ON)
option(BUILD_PLUGIN "Build a plugin library" OFF)

if (NOT BUILD_SHARED_LIBS AND NOT BUILD_STATIC_LIBS AND NOT BUILD_PLUGIN)
  message(FATAL_ERROR "One or all of BUILD_SHARED_LIBS, BUILD_STATIC_LIBS, or BUILD_PLUGIN must be set to ON to build")
endif()

set(CMAKE_CXX_STANDARD 11)
include(CTest)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

find_package(CURL)
include_directories(SYSTEM ${CURL_INCLUDE_DIRS})


include_directories(SYSTEM 3rd_party/include)

include_directories(zipkin/include)
add_subdirectory(zipkin)

include_directories(zipkin_opentracing/include)
add_subdirectory(zipkin_opentracing)
