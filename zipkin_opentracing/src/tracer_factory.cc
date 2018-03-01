#include "tracer_factory.h"
#include <exception>
#include <iostream>
#include <zipkin/opentracing.h>
#include <zipkin/rapidjson/error/en.h>
#include <zipkin/rapidjson/schema.h>

const char *const configuration_schema = R"(
{
  "$schema": "http://json-schema.org/schema#",
  "title": "TracerConfiguration",
  "type": "object",
  "required": ["service_name"],
  "properties": {
    "service_name": {
      "type": "string"
    },
    "service_address": {
      "type": "string"
    },
    "collector_host": {
      "type": "string"
    },
    "collector_port": {
      "type": "integer",
      "minimum": 1,
      "maximum": 65535
    },
    "reporting_period": {
      "type": "integer",
      "description": 
        "The time in microseconds between sending successive reports to the collector",
      "minimum": 1
    },
    "max_buffered_spans": {
      "type": "integer",
      "description":
        "The maximum number of spans to buffer before sending them to the collector",
      "minimum": 1
    }
  }
}
)";

namespace zipkin {
opentracing::expected<std::shared_ptr<opentracing::Tracer>>
OtTracerFactory::MakeTracer(const char *configuration,
                            std::string &error_message) const noexcept try {
  static rapidjson::SchemaDocument schema = [] {
    rapidjson::Document document;
    document.Parse(configuration_schema);
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
  if (document.HasMember("collector_host")) {
    options.collector_host = document["collector_host"].GetString();
  }
  if (document.HasMember("collector_port")) {
    options.collector_port = document["collector_port"].GetInt();
  }
  if (document.HasMember("reporting_period")) {
    options.reporting_period =
        std::chrono::microseconds{document["reporting_period"].GetInt()};
  }
  if (document.HasMember("max_buffered_spans")) {
    options.max_buffered_spans = document["max_buffered_spans"].GetInt();
  }
  return makeZipkinOtTracer(options);
} catch (const std::bad_alloc &) {
  return opentracing::make_unexpected(
      std::make_error_code(std::errc::not_enough_memory));
}
} // namespace zipkin
