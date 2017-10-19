find_path(OPENTRACING_INCLUDE_DIR NAMES opentracing/tracer.h)
find_library(OPENTRACING_LIB opentracing)

include_directories(SYSTEM ${OPENTRACING_INCLUDE_DIR})

install(DIRECTORY include/zipkin DESTINATION include)
add_library(zipkin_opentracing src/utility.cc
                               src/propagation.cc
                               src/opentracing.cc)
target_link_libraries(zipkin_opentracing ${CURL_LIBRARIES} ${OPENTRACING_LIB} zipkin)
install(TARGETS zipkin_opentracing
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

add_subdirectory(example)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
