#include <opentracing/tracer.h>
#include <zipkin/ip_address.h>
#include <zipkin/tracer.h>

namespace zipkin {
struct ZipkinOtTracerOptions {
  std::string collector_host = "localhost";
  uint32_t collector_port = 9411;
  SteadyClock::duration reporting_period = DEFAULT_REPORTING_PERIOD;
  size_t max_buffered_spans = DEFAULT_SPAN_BUFFER_SIZE;
  float sample_rate = 1.0f;

  std::string service_name;
  IpAddress service_address;
};

std::shared_ptr<opentracing::Tracer>
makeZipkinOtTracer(const ZipkinOtTracerOptions &options);

std::shared_ptr<opentracing::Tracer>
makeZipkinOtTracer(const ZipkinOtTracerOptions &options,
                   std::unique_ptr<Reporter> &&reporter);
} // namespace zipkin
