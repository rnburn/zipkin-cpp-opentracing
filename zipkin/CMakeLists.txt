install(DIRECTORY include/zipkin DESTINATION include)
set(ZIPKIN_SRCS src/zipkin_core_types.cc
                 src/utility.cc
                 src/hex.cc
                 src/tracer.cc
                 src/ip_address.cc
                 src/span_buffer.cc
                 src/span_context.cc
                 src/zipkin_reporter_impl.cc
                 src/zipkin_http_transporter.cc)

set(WIN32_LIBRARIES)
if(WIN32)
   list(APPEND WIN32_LIBRARIES wsock32.lib ws2_32.lib)
endif()

if (BUILD_SHARED_LIBS)               
  add_library(zipkin SHARED ${ZIPKIN_SRCS})
  target_link_libraries(zipkin Threads::Threads ${CURL_LIBRARIES} ${WIN32_LIBRARIES})
  set_target_properties(zipkin PROPERTIES VERSION ${ZIPKIN_VERSION_STRING}
                                          SOVERSION ${ZIPKIN_VERSION_MAJOR})
  install(TARGETS zipkin 
          LIBRARY DESTINATION lib)
endif()

if (BUILD_STATIC_LIBS)
  add_library(zipkin-static STATIC ${ZIPKIN_SRCS})
  set_target_properties(zipkin-static PROPERTIES OUTPUT_NAME zipkin)
  target_link_libraries(zipkin-static Threads::Threads ${CURL_LIBRARIES} ${WIN32_LIBRARIES})
  install(TARGETS zipkin-static
          ARCHIVE DESTINATION lib)
endif()

if (BUILD_PLUGIN)
  add_library(zipkin-plugin-static STATIC ${ZIPKIN_SRCS})
  target_link_libraries(zipkin-plugin-static Threads::Threads ${CURL_LIBRARIES} ${WIN32_LIBRARIES})
  set_property(TARGET zipkin-plugin-static PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()


if(BUILD_TESTING AND BUILD_SHARED_LIBS)
  add_subdirectory(test)
endif()
