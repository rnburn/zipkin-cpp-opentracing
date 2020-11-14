#include "../src/sampling.h"
#include "../src/utility.h"
#include "in_memory_reporter.h"
#include <algorithm>
#include <stdexcept>
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
        default:
          throw std::runtime_error("Unhandled annotation type");
        }
      });
}

static bool IsChildOf(const zipkin::Span &a, const zipkin::Span &b) {
  return a.isSetParentId() && a.parentId() == b.id() &&
         a.traceId() == b.traceId();
}

class TextMapCarrier : public ot::TextMapWriter {
public:
  TextMapCarrier(std::unordered_map<std::string, std::string> &text_map)
      : text_map_(text_map) {}

  ot::expected<void> Set(ot::string_view key, ot::string_view value) const override {
    text_map_[key] = value;
    return {};
  }

private:
  std::unordered_map<std::string, std::string> &text_map_;
};

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

  SECTION("Uses sampling rate to determine whether to sample a span.") {
    auto r = new InMemoryReporter();

    ZipkinOtTracerOptions options;
    options.sample_rate = 0.0;
    auto t = makeZipkinOtTracer(options, std::unique_ptr<Reporter>(r));

    auto span_a = t->StartSpan("a");
    CHECK(span_a);
    span_a->Finish();

    CHECK(r->spans().empty());
  }

  SECTION("Propagates sampling decision to child span") {
    ZipkinOtTracerOptions no_sampling;
    no_sampling.sample_rate = 0.0;
    auto r1 = new InMemoryReporter();
    auto no_sampling_tracer =
        makeZipkinOtTracer(no_sampling, std::unique_ptr<Reporter>(r1));

    ZipkinOtTracerOptions always_sample;
    always_sample.sample_rate = 1.0;
    auto r2 = new InMemoryReporter();
    auto sampling_tracer =
        makeZipkinOtTracer(always_sample, std::unique_ptr<Reporter>(r2));

    auto span_a = no_sampling_tracer->StartSpan("a");
    CHECK(span_a);
    span_a->Finish();
    auto span_b =
        sampling_tracer->StartSpan("b", {ChildOf(&span_a->context())});
    CHECK(span_b);
    span_b->Finish();

    CHECK(r1->spans().empty());
    CHECK(r2->spans().empty());
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

  SECTION("Get SpanContext and check that it is valid after Finish") {
    auto span = tracer->StartSpan("a");
    CHECK(span);
    span->SetTag("abc", 123);
    auto &ctx = span->context();
    span->Finish();
    auto trace_id = ctx.ToTraceID();
    CHECK(trace_id != "");
    auto span_id = ctx.ToSpanID();
    CHECK(span_id != "");
  }

  SECTION("Check SpanContext.Clone() preserves attributes") {
    auto span = tracer->StartSpan("a");
    CHECK(span);
    span->SetTag("abc", 123);
    span->SetBaggageItem("a", "1");
    auto &ctx = span->context();
    span->Finish();
    auto ctx2 = ctx.Clone();

    CHECK(ctx.ToTraceID() == ctx2->ToTraceID());
    CHECK(ctx.ToSpanID() == ctx2->ToSpanID());

    std::unordered_map<std::string, std::string> items;
    ctx.ForeachBaggageItem(
        [&items] (const std::string& key, const std::string& value) {
            items[key] = value;
            return true;
        }
    );

    std::unordered_map<std::string, std::string> items2;
    ctx2->ForeachBaggageItem(
        [&items2] (const std::string& key, const std::string& value) {
            items2[key] = value;
            return true;
        }
    );
    CHECK(items == items2);

    // Cross-check: serialize span context to a carrier
    // and compare low-level representation
    items.clear();
    items2.clear();

    TextMapCarrier carrier{items};
    tracer->Inject(ctx, carrier);
    TextMapCarrier carrier2{items2};
    tracer->Inject(*ctx2, carrier2);

    CHECK(items == items2);
  }
}
