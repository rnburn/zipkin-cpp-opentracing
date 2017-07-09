install(DIRECTORY include/zipkin DESTINATION include)
add_library(zipkin_opentracing src/opentracing.cc)
target_link_libraries(zipkin_opentracing zipkin ${CURL_LIBRARIES})
install(TARGETS zipkin_opentracing
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

add_subdirectory(test)
