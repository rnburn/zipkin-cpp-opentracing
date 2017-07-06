#include <zipkin/opentracing.h>

#include "tracer.h"
#include "zipkin_http_transporter.h"
#include "zipkin_reporter_impl.h"
#include "zipkin_core_types.h"

using opentracing::string_view;
using opentracing::Value;
using opentracing::expected;
using opentracing::make_unexpected;

namespace ot = opentracing;

namespace zipkin {
class OtSpanContext : public ot::SpanContext {
 public:
  void ForeachBaggageItem(
      std::function<bool(const std::string&, const std::string&)> f)
      const override {}
};

class OtSpan : public ot::Span {
 public:
  OtSpan(std::shared_ptr<const ot::Tracer>&& tracer, SpanPtr&& span)
      : tracer_{std::move(tracer)}, span_{std::move(span)} {}

  void FinishWithOptions(
      const ot::FinishSpanOptions& options) noexcept override {
    span_->finish();
  }

  void SetOperationName(string_view name) noexcept override {
    span_->setName(name);
  }

  void SetTag(string_view restricted_key, const Value& value) noexcept override {
  }

  void SetBaggageItem(string_view restricted_key,
                      string_view value) noexcept override {}

  std::string BaggageItem(string_view restricted_key) const noexcept override {
    return {};
  }

  void Log(std::initializer_list<std::pair<string_view, Value>>
               fields) noexcept override {}

  const ot::SpanContext& context() const noexcept override {}

  const ot::Tracer& tracer() const noexcept override { return *tracer_; }

 private:
  std::shared_ptr<const ot::Tracer> tracer_;
  SpanPtr span_;
};

class OtTracer : public ot::Tracer,
                 public std::enable_shared_from_this<OtTracer> {
 public:
  explicit OtTracer(TracerPtr&& tracer) : tracer_{std::move(tracer)} {}

  std::unique_ptr<ot::Span> StartSpanWithOptions(
      string_view operation_name, const ot::StartSpanOptions& options) const
      noexcept override {
    auto span =
        tracer_->startSpan(operation_name, options.start_system_timestamp);
    span->setTracer(tracer_.get());
    return std::unique_ptr<ot::Span>{
        new OtSpan{shared_from_this(), std::move(span)}};
  }

  expected<void> Inject(const ot::SpanContext& sc,
                        std::ostream& writer) const override {
    return {};
  }

  expected<void> Inject(const ot::SpanContext& sc,
                        const ot::TextMapWriter& writer) const override {
    return {};
  }

  expected<void> Inject(const ot::SpanContext& sc,
                        const ot::HTTPHeadersWriter& writer) const override {
    return {};
  }

  expected<std::unique_ptr<ot::SpanContext>> Extract(
      std::istream& reader) const override {
    return std::unique_ptr<ot::SpanContext>{};
  }

  expected<std::unique_ptr<ot::SpanContext>> Extract(
      const ot::TextMapReader& reader) const override {
    return std::unique_ptr<ot::SpanContext>{};
  }

  expected<std::unique_ptr<ot::SpanContext>> Extract(
      const ot::HTTPHeadersReader& reader) const override {
    return std::unique_ptr<ot::SpanContext>{};
  }

  void Close() noexcept override {}
 private:
  TracerPtr tracer_;
};

std::shared_ptr<ot::Tracer> makeZipkinTracer(
    const ZipkinTracerOptions& options) {
  TracerPtr tracer{new Tracer{options.service_name, options.service_address}};
  TransporterPtr transporter{new ZipkinHttpTransporter{
      options.collector_host.c_str(), options.collector_port}};
  ReporterPtr reporter{new ReporterImpl{std::move(transporter)}};
  tracer->setReporter(std::move(reporter));
  return std::shared_ptr<ot::Tracer>{new OtTracer{std::move(tracer)}};
}
}  // namespace zipkin
