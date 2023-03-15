#include "tracer_factory.h"
#include <exception>
#include <iostream>
#include <zipkin/opentracing.h>
#include <zipkin/rapidjson/error/en.h>
#include <zipkin/rapidjson/schema.h>

namespace zipkin {

extern const unsigned char tracer_configuration_schema[];
extern const int tracer_configuration_schema_size;

opentracing::expected<std::shared_ptr<opentracing::Tracer>>
OtTracerFactory::MakeTracer(const char *configuration,
                            std::string &error_message) const noexcept try {
  static rapidjson::SchemaDocument schema = [] {
    rapidjson::Document document;
    document.Parse(reinterpret_cast<const char *>(tracer_configuration_schema),
                   tracer_configuration_schema_size);
    if (document.HasParseError()) {
      std::cerr << "Internal Error: Configuration schema is invalid.\n";
      std::terminate();
    }
    return rapidjson::SchemaDocument{document};
  }();

  rapidjson::Document document;
  rapidjson::ParseResult parse_result = document.Parse(configuration);
  if (!parse_result) {
    error_message = std::string{"JSON parse error: "} +
                    rapidjson::GetParseError_En(parse_result.Code()) + " (" +
                    std::to_string(parse_result.Offset()) + ")";
    return opentracing::make_unexpected(opentracing::configuration_parse_error);
  }

  rapidjson::SchemaValidator validator{schema};
  if (!document.Accept(validator)) {
    error_message = "Configuration is invalid.";
    return opentracing::make_unexpected(
        opentracing::invalid_configuration_error);
  }

  ZipkinOtTracerOptions options;
  options.service_name = document["service_name"].GetString();
  if (document.HasMember("service_address")) {
    options.service_address =
        IpAddress{IpVersion::v4, document["service_address"].GetString()};
  }
  if (document.HasMember("collector_base_url")) {
    options.collector_base_url = document["collector_base_url"].GetString();
  }
  if (document.HasMember("collector_timeout")) {
    options.collector_timeout =
            std::chrono::milliseconds{document["collector_timeout"].GetInt()};
  }
  if (document.HasMember("reporting_period")) {
    options.reporting_period =
        std::chrono::microseconds{document["reporting_period"].GetInt()};
  }
  if (document.HasMember("max_buffered_spans")) {
    options.max_buffered_spans = document["max_buffered_spans"].GetInt();
  }
  if (document.HasMember("sample_rate")) {
    options.sample_rate = document["sample_rate"].GetDouble();
  }
  return makeZipkinOtTracer(options);
} catch (const std::bad_alloc &) {
  return opentracing::make_unexpected(
      std::make_error_code(std::errc::not_enough_memory));
}
} // namespace zipkin
