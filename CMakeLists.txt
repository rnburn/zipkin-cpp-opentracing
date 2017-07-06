cmake_minimum_required(VERSION 3.0)
enable_testing()

project(zipkin-opentracing)
set(CMAKE_CXX_STANDARD 11)

find_package(CURL)
include_directories(SYSTEM ${CURL_INCLUDE_DIRS})


include_directories(SYSTEM 3rd_party/include)

include_directories(include)
install(DIRECTORY include/zipkin DESTINATION include)
add_library(zipkin src/zipkin_core_types.cc
                   src/utility.cc
                   src/hex.cc
                   src/tracer.cc
                   src/ip_address.cc
                   src/span_buffer.cc
                   src/span_context.cc
                   src/zipkin_reporter_impl.cc
                   src/zipkin_http_transporter.cc
                   src/opentracing.cc)
target_link_libraries(zipkin ${CURL_LIBRARIES})
install(TARGETS zipkin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

add_subdirectory(test)
