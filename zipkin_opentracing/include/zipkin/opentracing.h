#include <opentracing/tracer.h>
#include <zipkin/ip_address.h>
#include <zipkin/tracer.h>

namespace zipkin {
class OtTracer : public opentracing::Tracer,
                 public std::enable_shared_from_this<OtTracer> {
public:
  explicit OtTracer(TracerPtr &&tracer);

  std::unique_ptr<opentracing::Span>
  StartSpanWithOptions(opentracing::string_view operation_name,
                       const opentracing::StartSpanOptions &options) const
      noexcept override;

  opentracing::expected<void> Inject(const opentracing::SpanContext &sc,
                                     std::ostream &writer) const override;

  opentracing::expected<void>
  Inject(const opentracing::SpanContext &sc,
         const opentracing::TextMapWriter &writer) const override;

  opentracing::expected<void>
  Inject(const opentracing::SpanContext &sc,
         const opentracing::HTTPHeadersWriter &writer) const override;

  opentracing::expected<std::unique_ptr<opentracing::SpanContext>>
  Extract(std::istream &reader) const override;

  opentracing::expected<std::unique_ptr<opentracing::SpanContext>>
  Extract(const opentracing::TextMapReader &reader) const override;

  opentracing::expected<std::unique_ptr<opentracing::SpanContext>>
  Extract(const opentracing::HTTPHeadersReader &reader) const override;

  void Close() noexcept override;

private:
  TracerPtr tracer_;

  template <class Carrier>
  opentracing::expected<void> InjectImpl(const opentracing::SpanContext &sc,
                                         Carrier &writer) const;

  template <class Carrier>
  opentracing::expected<std::unique_ptr<opentracing::SpanContext>>
  ExtractImpl(Carrier &reader) const;
};

struct ZipkinOtTracerOptions {
  std::string collector_host = "localhost";
  uint32_t collector_port = 9411;
  SteadyClock::duration reporting_period = DEFAULT_REPORTING_PERIOD;
  size_t max_buffered_spans = DEFAULT_SPAN_BUFFER_SIZE;

  std::string service_name;
  IpAddress service_address;
};

std::shared_ptr<opentracing::Tracer>
makeZipkinOtTracer(const ZipkinOtTracerOptions &options);

std::shared_ptr<opentracing::Tracer>
makeZipkinOtTracer(const ZipkinOtTracerOptions &options,
                   std::unique_ptr<Reporter> &&reporter);
} // namespace zipkin
