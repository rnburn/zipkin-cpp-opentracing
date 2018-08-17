set(CPACK_RPM_COMPONENT_INSTALL ON)
cmake_minimum_required(VERSION 3.0)

project(zipkin-opentracing)

set(ZIPKIN_VERSION_MAJOR "0")
set(ZIPKIN_VERSION_MINOR "5")
set(ZIPKIN_VERSION_PATCH "2")
set(ZIPKIN_VERSION_STRING
  "${ZIPKIN_VERSION_MAJOR}.${ZIPKIN_VERSION_MINOR}.${ZIPKIN_VERSION_PATCH}")

# ==============================================================================
# Set up cpack

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ implementation of Zipkin")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")

SET(CPACK_PACKAGE_VERSION_MAJOR ${ZIPKIN_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${ZIPKIN_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${ZIPKIN_VERSION_PATCH})
set(CPACK_RPM_DIST_POST_INSTALL_SCRIPT_FILE ${CMAKE_SOURCE_DIR}/scripts/runldconfig)
set(CPACK_RPM_DIST_POST_UNINSTALL_SCRIPT_FILE ${CMAKE_SOURCE_DIR}/scripts/runldconfig)
set(CPACK_COMPONENTS_ALL DIST DEVEL)
set(CPACK_COMPONENTS_GROUPING ONE_PER_GROUP)
set(CPACK_GENERATOR "RPM")
set(CPACK_COMPONENTS_IGNORE_GROUPS 1)

set(CPACK_COMPONENT_DIST_REQUIRED TRUE)
set(CPACK_COMPONENT_DEVEL_REQUIRED TRUE)

include(CPack)
# ==============================================================================

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

add_executable(embedfile 3rd_party/embedfile/src/embedfile.c)

include_directories(zipkin/include)
add_subdirectory(zipkin)

include_directories(zipkin_opentracing/include)
add_subdirectory(zipkin_opentracing)
