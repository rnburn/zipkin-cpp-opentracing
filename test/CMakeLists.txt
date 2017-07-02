add_executable(tracer_test tracer_test.cc)
add_test(tracer_test tracer_test)
target_link_libraries(tracer_test zipkin)
