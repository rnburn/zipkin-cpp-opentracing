#include "in_memory_reporter.h"
#include <zipkin/opentracing.h>

#define CATCH_CONFIG_MAIN
#include <zipkin/catch/catch.hpp>
using namespace zipkin;

TEST_CASE("tracer") {
  auto reporter = new InMemoryReporter();
  ZipkinOtTracerOptions options;
  auto tracer =
      makeZipkinOtTracer(options, std::unique_ptr<Reporter>(reporter));

  SECTION("The operation name can be changed after the span is started.") {
    auto span = tracer->StartSpan("a");
    CHECK(span);
    span->SetOperationName("b");
    span->Finish();
    CHECK(reporter->top().name() == "b");
  }
}
