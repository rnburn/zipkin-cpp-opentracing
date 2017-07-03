#include <zipkin/opentracing.h>

using opentracing::StringRef;
using opentracing::Value;
using opentracing::Expected;
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
  void FinishWithOptions(
      const ot::FinishSpanOptions& options) noexcept override {}

  void SetOperationName(StringRef name) noexcept override {}

  void SetBaggageItem(StringRef restricted_key,
                      StringRef value) noexcept override {}

  void Log(std::initializer_list<std::pair<StringRef, Value>>
               fields) noexcept override {}

  const ot::SpanContext& context() const noexcept override {}

  const ot::Tracer& tracer() const noexcept override {}
};

class OtTracer : public ot::Tracer {
 public:
  std::unique_ptr<ot::Span> StartSpanWithOptions(
      StringRef operation_name, const ot::StartSpanOptions& options) const
      noexcept override {
    return nullptr;
  }

  Expected<void> Inject(const ot::SpanContext& sc,
                        std::ostream& writer) const override {
    return {};
  }

  Expected<void> Inject(const ot::SpanContext& sc,
                        const ot::TextMapWriter& writer) const override {
    return {};
  }

  Expected<void> Inject(const ot::SpanContext& sc,
                        const ot::HTTPHeadersWriter& writer) const override {
    return {};
  }

  Expected<std::unique_ptr<ot::SpanContext>> Extract(
      std::istream& reader) const override {
    return std::unique_ptr<ot::SpanContext>{};
  }

  Expected<std::unique_ptr<ot::SpanContext>> Extract(
      const ot::TextMapReader& reader) const override {
    return std::unique_ptr<ot::SpanContext>{};
  }

  Expected<std::unique_ptr<ot::SpanContext>> Extract(
      const ot::HTTPHeadersReader& reader) const override {
    return std::unique_ptr<ot::SpanContext>{};
  }

  void Close() noexcept override {}
};

std::shared_ptr<ot::Tracer> makeZipkinTracer(
    const ZipkinTracerOptions& options) {
  return nullptr;
}
}  // namespace zipkin
