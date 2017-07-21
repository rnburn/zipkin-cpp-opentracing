add_executable(hex_test hex_test.cc)
add_test(hex_test hex_test)
target_link_libraries(hex_test zipkin)
