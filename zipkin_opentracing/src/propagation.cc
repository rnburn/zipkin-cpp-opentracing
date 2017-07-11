#include "propagation.h"

namespace ot = opentracing;

namespace zipkin {
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
  return ot::make_unexpected(ot::invalid_carrier_error);
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
