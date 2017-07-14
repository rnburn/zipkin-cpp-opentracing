add_executable(tutorial tutorial.cc)
target_link_libraries(tutorial ${CURL_LIBRARIES} ${OPENTRACING_LIB} zipkin zipkin_opentracing)
