install(DIRECTORY include/zipkin DESTINATION include)
add_library(zipkin src/zipkin_core_types.cc
                   src/utility.cc
                   src/hex.cc
                   src/tracer.cc
                   src/ip_address.cc
                   src/span_buffer.cc
                   src/span_context.cc
                   src/zipkin_reporter_impl.cc
                   src/zipkin_http_transporter.cc)
target_link_libraries(zipkin Threads::Threads ${CURL_LIBRARIES})
install(TARGETS zipkin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

add_subdirectory(test)

