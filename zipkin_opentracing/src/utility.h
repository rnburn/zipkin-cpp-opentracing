#pragma once

#include <opentracing/string_view.h>
#include <opentracing/value.h>
#include <zipkin/zipkin_core_types.h>

namespace zipkin {
BinaryAnnotation toBinaryAnnotation(opentracing::string_view key,
                                    const opentracing::Value &value);
} // namespace zipkin
