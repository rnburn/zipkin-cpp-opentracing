cc_library(
    name = "rapidjson",
    hdrs = glob(["3rd_party/include/zipkin/rapidjson/**/*.h"]),
    strip_include_prefix = "3rd_party/include",
)

cc_library(
    name = "randutils",
    hdrs = glob(["3rd_party/include/zipkin/randutils/**/*.h"]),
    strip_include_prefix = "3rd_party/include",
)

cc_library(
    name = "zipkin",
    srcs = glob(["zipkin/src/*.cc", "zipkin/src/*.h"]),
    hdrs = glob(["zipkin/include/zipkin/*.h"]),
    strip_include_prefix = "zipkin/include",
    visibility = ["//visibility:public"],
    deps = [
        ":rapidjson",
        ":randutils",
        "@se_haxx_curl_libcurl//:curl",
    ]
)

cc_library(
    name = "zipkin_opentracing",
    srcs = glob(["zipkin_opentracing/src/*.cc", "zipkin_opentracing/src/*.h"]),
    hdrs = glob(["zipkin_opentracing/include/zipkin/*.h"]),
    strip_include_prefix = "zipkin_opentracing/include",
    visibility = ["//visibility:public"],
    deps = [
        ":zipkin",
        ":randutils",
        "@io_opentracing_cpp//:opentracing"
    ]
)
