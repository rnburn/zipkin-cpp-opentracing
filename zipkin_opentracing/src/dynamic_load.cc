#include "tracer_factory.h"
#include <cstring>
#include <opentracing/dynamic_load.h>

int OpenTracingMakeTracerFactory(const char *opentracing_version,
                                 const void **error_category,
                                 void **tracer_factory) {
  if (std::strcmp(opentracing_version, OPENTRACING_VERSION) != 0) {
    *error_category =
        static_cast<const void *>(&opentracing::dynamic_load_error_category());
    return opentracing::incompatible_library_versions_error.value();
  }

  *tracer_factory = new (std::nothrow) zipkin::OtTracerFactory{};
  if (tracer_factory == nullptr) {
    *error_category = static_cast<const void *>(&std::generic_category());
    return static_cast<int>(std::errc::not_enough_memory);
  }

  return 0;
}
