#include "../src/utility.h"
#include "in_memory_reporter.h"
#include <algorithm>
#include <zipkin/opentracing.h>

#define CATCH_CONFIG_MAIN
#include <zipkin/catch/catch.hpp>
using namespace zipkin;
namespace ot = opentracing;

static bool hasTag(const Span &span, ot::string_view key, ot::Value value) {
  auto tag_annotation = toBinaryAnnotation(key, value);
  return std::any_of(
      std::begin(span.binaryAnnotations()), std::end(span.binaryAnnotations()),
      [&](const BinaryAnnotation &annotation) {
        if (tag_annotation.key() != annotation.key() ||
            tag_annotation.annotationType() != annotation.annotationType()) {
          return false;
        }
        switch (tag_annotation.annotationType()) {
        case BOOL:
          return tag_annotation.valueBool() == annotation.valueBool();
        case STRING:
          return tag_annotation.valueString() == annotation.valueString();
        case INT64:
          return tag_annotation.valueInt64() == annotation.valueInt64();
        case DOUBLE:
          return tag_annotation.valueDouble() == annotation.valueDouble();
        }
      });
}

TEST_CASE("ot_tracer") {
  auto reporter = new InMemoryReporter();
  ZipkinOtTracerOptions options;
  auto tracer =
      makeZipkinOtTracer(options, std::unique_ptr<Reporter>(reporter));

  SECTION("StartSpan applies the provided tags.") {
    {
      auto span = tracer->StartSpan(
          "a", {ot::SetTag("abc", 123), ot::SetTag("xyz", true)});
      CHECK(span);
      span->Finish();
    }
    auto span = reporter->top();
    CHECK(span.name() == "a");
    CHECK(hasTag(span, "abc", 123));
    CHECK(hasTag(span, "xyz", true));
  }

  SECTION("The operation name can be changed after the span is started.") {
    auto span = tracer->StartSpan("a");
    CHECK(span);
    span->SetOperationName("b");
    span->Finish();
    CHECK(reporter->top().name() == "b");
  }

  SECTION("Baggage from the span references are copied over to a new span "
          "context") {
    auto span_a = tracer->StartSpan("a");
    CHECK(span_a);
    span_a->SetBaggageItem("a", "1");
    auto span_b = tracer->StartSpan("b", {ot::ChildOf(&span_a->context())});
    CHECK(span_b);
    CHECK(span_b->BaggageItem("a") == "1");
  }
}
