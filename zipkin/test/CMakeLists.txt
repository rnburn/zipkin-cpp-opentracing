add_executable(tracer_test tracer_test.cc)
add_test(tracer_test tracer_test)
target_link_libraries(tracer_test zipkin)

add_executable(hex_test hex_test.cc)
add_test(hex_test hex_test)
target_link_libraries(hex_test zipkin)
