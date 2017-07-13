#pragma once

#include <opentracing/propagation.h>
#include <unordered_map>
#include <zipkin/span_context.h>

namespace zipkin {
opentracing::expected<void>
injectSpanContext(std::ostream &carrier,
                  const zipkin::SpanContext &span_context,
                  const std::unordered_map<std::string, std::string> &baggage);

opentracing::expected<void>
injectSpanContext(const opentracing::TextMapWriter &carrier,
                  const zipkin::SpanContext &span_context,
                  const std::unordered_map<std::string, std::string> &baggage);

opentracing::expected<Optional<zipkin::SpanContext>>
extractSpanContext(std::istream &carrier,
                   std::unordered_map<std::string, std::string> &baggage);

opentracing::expected<Optional<zipkin::SpanContext>>
extractSpanContext(const opentracing::TextMapReader &carrier,
                   std::unordered_map<std::string, std::string> &baggage);
} // namespace zipkin
