#pragma once

#include <opentracing/tracer_factory.h>

namespace zipkin {
class OtTracerFactory : public opentracing::TracerFactory {
public:
  opentracing::expected<std::shared_ptr<opentracing::Tracer>>
  MakeTracer(const char *configuration, std::string &error_message) const
      noexcept override;
};
} // namespace zipkin
