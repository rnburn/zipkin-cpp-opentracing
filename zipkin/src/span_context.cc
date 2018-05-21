#include <zipkin/span_context.h>

#include "zipkin_core_constants.h"
#include <zipkin/utility.h>

namespace zipkin {
SpanContext::SpanContext(const Span &span) {
  trace_id_ = span.traceId();
  id_ = span.id();
  parent_id_ = span.isSetParentId() ? span.parentId() : 0;

  if (span.isSampled()) {
    flags_ |= static_cast<unsigned char>(zipkin::sampled_flag);
    flags_ |= static_cast<unsigned char>(zipkin::sampling_set_flag);
  }

  for (const Annotation &annotation : span.annotations()) {
    if (annotation.value() == ZipkinCoreConstants::get().CLIENT_RECV) {
      annotation_values_.cr_ = true;
    } else if (annotation.value() == ZipkinCoreConstants::get().CLIENT_SEND) {
      annotation_values_.cs_ = true;
    } else if (annotation.value() == ZipkinCoreConstants::get().SERVER_RECV) {
      annotation_values_.sr_ = true;
    } else if (annotation.value() == ZipkinCoreConstants::get().SERVER_SEND) {
      annotation_values_.ss_ = true;
    }
  }

  is_initialized_ = true;
}
} // namespace zipkin
