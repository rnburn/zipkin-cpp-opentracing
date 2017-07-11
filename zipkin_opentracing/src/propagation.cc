#include "propagation.h"

#include <string>

namespace ot = opentracing;

namespace zipkin {
#define PREFIX_TRACER_STATE "x-b3-"
// Note: these constants are a convention of the OpenTracing basictracers.
const ot::string_view prefix_baggage = "ot-baggage-";

const int tracer_state_field_count = 3;
const ot::string_view zipkin_trace_id = PREFIX_TRACER_STATE "traceid";
const ot::string_view zipkin_span_id = PREFIX_TRACER_STATE "spanid";
const ot::string_view zipkin_parent_span_id =
    PREFIX_TRACER_STATE "parentspanid";
const ot::string_view zipkin_sampled = PREFIX_TRACER_STATE "sampled";
const ot::string_view zipkin_flags = PREFIX_TRACER_STATE "flags";
#undef PREFIX_TRACER_STATE

opentracing::expected<void>
injectSpanContext(std::ostream &carrier,
                  const zipkin::SpanContext &span_context,
                  const std::unordered_map<std::string, std::string> &baggage) {
  return ot::make_unexpected(ot::invalid_carrier_error);
}

opentracing::expected<void>
injectSpanContext(const opentracing::TextMapWriter &carrier,
                  const zipkin::SpanContext &span_context,
                  const std::unordered_map<std::string, std::string> &baggage) {
  auto result = carrier.Set(zipkin_trace_id, span_context.traceIdAsHexString());
  if (!result) {
    return result;
  }
  result = carrier.Set(zipkin_span_id, span_context.idAsHexString());
  if (!result) {
    return result;
  }
  result = carrier.Set(zipkin_sampled, "true");
  if (!result) {
    return result;
  }
  if (span_context.isSetParentId()) {
    result =
        carrier.Set(zipkin_parent_span_id, span_context.parentIdAsHexString());
    if (!result) {
      return result;
    }
  }
  result = carrier.Set(zipkin_flags,
                       std::to_string(span_context.flags() & debug_flag));
  if (!result) {
    return result;
  }
  std::string baggage_key = prefix_baggage;
  for (const auto &baggage_item : baggage) {
    baggage_key.replace(std::begin(baggage_key) + prefix_baggage.size(),
                        std::end(baggage_key), baggage_item.first);
    result = carrier.Set(baggage_key, baggage_item.second);
    if (!result) {
      return result;
    }
  }
  return result;
}

opentracing::expected<zipkin::SpanContext>
extractSpanContext(std::istream &carrier,
                   std::unordered_map<std::string, std::string> &baggage) {
  return ot::make_unexpected(ot::invalid_carrier_error);
}

opentracing::expected<zipkin::SpanContext>
extractSpanContext(const opentracing::TextMapReader &carrier,
                   std::unordered_map<std::string, std::string> &baggage) {
  return ot::make_unexpected(ot::invalid_carrier_error);
}
} // namespace zipkin
