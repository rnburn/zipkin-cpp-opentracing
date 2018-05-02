find_path(OPENTRACING_INCLUDE_DIR NAMES opentracing/tracer.h)
find_library(OPENTRACING_LIB opentracing)

include_directories(SYSTEM ${OPENTRACING_INCLUDE_DIR})

install(DIRECTORY include/zipkin DESTINATION include)
set(ZIPKIN_OPENTRACING_SRCS src/utility.cc
                            src/propagation.cc
                            src/dynamic_load.cc
                            src/tracer_factory.cc
                            src/opentracing.cc)

if (BUILD_SHARED_LIBS)               
  add_library(zipkin_opentracing SHARED ${ZIPKIN_OPENTRACING_SRCS})
  target_link_libraries(zipkin_opentracing ${OPENTRACING_LIB} zipkin)
  set_target_properties(zipkin_opentracing PROPERTIES VERSION ${ZIPKIN_VERSION_STRING}
                                                      SOVERSION ${ZIPKIN_VERSION_MAJOR})
  install(TARGETS zipkin_opentracing 
          LIBRARY DESTINATION lib)
endif()

if (BUILD_STATIC_LIBS)
  add_library(zipkin_opentracing-static STATIC ${ZIPKIN_OPENTRACING_SRCS})
  set_target_properties(zipkin_opentracing-static PROPERTIES OUTPUT_NAME zipkin_opentracing)
  target_link_libraries(zipkin_opentracing-static ${OPENTRACING_LIB} zipkin)
  install(TARGETS zipkin_opentracing-static
          ARCHIVE DESTINATION lib)
endif()

if (BUILD_PLUGIN)
  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/export.map
    "{ global: OpenTracingMakeTracerFactory; local: *; };")
  add_library(zipkin_opentracing_plugin MODULE ${ZIPKIN_OPENTRACING_SRCS})
  target_link_libraries(zipkin_opentracing_plugin ${OPENTRACING_LIB}
    zipkin-plugin-static
    -static-libgcc
    -static-libstdc++
    -Wl,--version-script=${CMAKE_CURRENT_BINARY_DIR}/export.map)
  install(TARGETS zipkin_opentracing_plugin
          LIBRARY DESTINATION lib)
endif()


if(BUILD_SHARED_LIBS AND BUILD_TESTING)
  add_subdirectory(example)
  add_subdirectory(test)
endif()
