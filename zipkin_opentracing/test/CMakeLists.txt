macro(_zipkin_ot_test TEST_NAME)
  add_executable(${TEST_NAME} ${ARGN})
  target_link_libraries(${TEST_NAME} ${CURL_LIBRARIES} 
                                     ${OPENTRACING_LIB} 
                                     zipkin
                                     zipkin_opentracing)
  add_test(${TEST_NAME} ${TEST_NAME})
endmacro()

_zipkin_ot_test(ot_tracer_test tracer_test.cc)
