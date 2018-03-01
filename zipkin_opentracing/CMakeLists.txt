find_path(OPENTRACING_INCLUDE_DIR NAMES opentracing/tracer.h)
find_library(OPENTRACING_LIB opentracing)

include_directories(SYSTEM ${OPENTRACING_INCLUDE_DIR})

install(DIRECTORY include/zipkin DESTINATION include)
set(ZIPKIN_OPENTRACING_SRCS src/utility.cc
                            src/propagation.cc
                            src/dynamic_load.cc
                            src/tracer_factory.cc
                            src/opentracing.cc)

add_library(zipkin_opentracing SHARED ${ZIPKIN_OPENTRACING_SRCS})
target_link_libraries(zipkin_opentracing ${OPENTRACING_LIB} zipkin)
set_target_properties(zipkin_opentracing PROPERTIES VERSION ${ZIPKIN_VERSION_STRING}
                                                    SOVERSION ${ZIPKIN_VERSION_MAJOR})

add_library(zipkin_opentracing-static STATIC ${ZIPKIN_OPENTRACING_SRCS})
set_target_properties(zipkin_opentracing-static PROPERTIES OUTPUT_NAME zipkin_opentracing)
target_link_libraries(zipkin_opentracing-static ${OPENTRACING_LIB} zipkin)

install(TARGETS zipkin_opentracing zipkin_opentracing-static
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

add_subdirectory(example)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
