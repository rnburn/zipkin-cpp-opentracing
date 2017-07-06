#include <opentracing/tracer.h>
#include <zipkin/ip_address.h>

namespace zipkin {
struct ZipkinTracerOptions {
  std::string collector_host;
  uint32_t collector_port;

  std::string service_name;
  IpAddress service_address;
};

std::shared_ptr<opentracing::Tracer>
makeZipkinTracer(const ZipkinTracerOptions &options);
} // namespace zipkin
