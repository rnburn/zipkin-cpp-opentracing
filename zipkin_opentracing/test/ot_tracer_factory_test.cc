#include "../src/tracer_factory.h"

#define CATCH_CONFIG_MAIN
#include <zipkin/catch/catch.hpp>
using namespace zipkin;
namespace ot = opentracing;

TEST_CASE("OtTracerFactory") {
  OtTracerFactory tracer_factory;
  std::string error_message;

  SECTION("Construction a tracer from invalid json fails.") {
    const char *configuration = R"(
    {
      1 + 2
    )";
    auto tracer_maybe = tracer_factory.MakeTracer(configuration, error_message);
    CHECK(error_message != "");
    CHECK(!tracer_maybe);
  }

  SECTION("Construction a tracer from valid json, but an invalid configuration "
          "fails.") {
    const char *configuration = R"(
    {
      "service_name": "abc"
    })";
    auto tracer_maybe = tracer_factory.MakeTracer(configuration, error_message);
    CHECK(error_message != "");
    CHECK(!tracer_maybe);
  }

  SECTION("Constructing tracer with sample rate") {
    const char *configuration = R"(
    {
      "service_name": "abc",
      "collector_base_url": "http://foo.bar:80",
      "sample_rate": 0.1
    })";
    auto tracer_maybe = tracer_factory.MakeTracer(configuration, error_message);
    CHECK(error_message == "");
    CHECK(tracer_maybe);
  }

  SECTION("Constructing a tracer from a valid configuration succeeds.") {
    const char *configuration = R"(
    {
      "service_name": "abc"
    }
    )";
    auto tracer_maybe = tracer_factory.MakeTracer(configuration, error_message);
    CHECK(error_message == "");
    CHECK(tracer_maybe);
  }
}
