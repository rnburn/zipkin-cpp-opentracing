#include "../src/utility.h"
#include "in_memory_reporter.h"
#include <algorithm>
#include <opentracing/noop.h>
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

static bool IsChildOf(const zipkin::Span &a, const zipkin::Span &b) {
  return a.isSetParentId() && a.parentId() == b.id() &&
         a.traceId() == b.traceId();
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

  SECTION("You can set a single child-of reference when starting a span.") {
    auto span_a = tracer->StartSpan("a");
    CHECK(span_a);
    span_a->Finish();
    auto span_b = tracer->StartSpan("b", {ChildOf(&span_a->context())});
    CHECK(span_b);
    span_b->Finish();
    auto spans = reporter->spans();
    CHECK(IsChildOf(spans.at(1), spans.at(0)));
  }

  SECTION("You can set a single follows-from reference when starting a span.") {
    auto span_a = tracer->StartSpan("a");
    CHECK(span_a);
    span_a->Finish();
    auto span_b = tracer->StartSpan("b", {FollowsFrom(&span_a->context())});
    CHECK(span_b);
    span_b->Finish();
    auto spans = reporter->spans();
    // FollowsFrom is treated the same as ChildOf.
    CHECK(IsChildOf(spans.at(1), spans.at(0)));
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

  SECTION("References to non-Zipkin spans and null pointers are ignored.") {
    auto noop_tracer = ot::MakeNoopTracer();
    auto noop_span = noop_tracer->StartSpan("noop");
    CHECK(noop_span);
    ot::StartSpanOptions options;
    options.references.push_back(std::make_pair(
        ot::SpanReferenceType::ChildOfRef, &noop_span->context()));
    options.references.push_back(
        std::make_pair(ot::SpanReferenceType::ChildOfRef, nullptr));
    auto span = tracer->StartSpanWithOptions("a", options);
    CHECK(span);
    span->Finish();
    CHECK(!reporter->top().isSetParentId());
  }

  SECTION("Calling Finish a second time does nothing.") {
    auto span = tracer->StartSpan("a");
    CHECK(span);
    span->Finish();
    CHECK(reporter->size() == 1);
    span->Finish();
    CHECK(reporter->size() == 1);
  }

  SECTION("The operation name can be changed after the span is started.") {
    auto span = tracer->StartSpan("a");
    CHECK(span);
    span->SetOperationName("b");
    span->Finish();
    CHECK(reporter->top().name() == "b");
  }

  SECTION("Tags can be specified after a span is started.") {
    auto span = tracer->StartSpan("a");
    CHECK(span);
    span->SetTag("abc", 123);
    span->Finish();
    CHECK(hasTag(reporter->top(), "abc", 123));
  }
}
