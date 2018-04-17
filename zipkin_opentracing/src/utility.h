#pragma once

#include <opentracing/string_view.h>
#include <opentracing/value.h>
#include <zipkin/zipkin_core_types.h>

namespace zipkin {

BinaryAnnotation toBinaryAnnotation(opentracing::string_view key,
                                    const opentracing::Value &value);

Annotation toAnnotation(const std::vector<std::pair<opentracing::string_view, opentracing::Value>>& fields);

} // namespace zipkin
