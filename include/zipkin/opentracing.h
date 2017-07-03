#include <opentracing/tracer.h>

namespace zipkin {
struct ZipkinTracerOptions {};

std::shared_ptr<opentracing::Tracer>
makeZipkinTracer(const ZipkinTracerOptions &options);
} // namespace zipkin
