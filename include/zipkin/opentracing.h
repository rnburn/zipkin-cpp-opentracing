#include <opentracing/tracer.h>

namespace zipkin {
struct ZipkinTracerOptions {
  std::string collector_host;
  uint32_t collector_port;

  std::string service_name;
  std::string service_address;
  uint32_t service_port;
};

std::shared_ptr<opentracing::Tracer>
makeZipkinTracer(const ZipkinTracerOptions &options);
} // namespace zipkin
