#include <zipkin/tracer.h>

#include <chrono>

#include "zipkin_core_constants.h"
#include <zipkin/utility.h>

namespace zipkin {
SpanPtr Tracer::startSpan(const std::string &span_name, SystemTime timestamp) {
  // Build the endpoint
  Endpoint ep(service_name_, address_);

  // Create an all-new span, with no parent id
  SpanPtr span_ptr(new Span());
  span_ptr->setName(span_name);
  TraceId random_number(RandomUtil::generateId(), RandomUtil::generateId());
  span_ptr->setId(random_number.low());
  span_ptr->setTraceId(random_number);
  int64_t start_time_micro =
      std::chrono::duration_cast<std::chrono::microseconds>(
          SteadyClock::now().time_since_epoch())
          .count();
  span_ptr->setStartTime(start_time_micro);

  // Set the timestamp globally for the span and also for the CS annotation
  uint64_t timestamp_micro =
      std::chrono::duration_cast<std::chrono::microseconds>(
          timestamp.time_since_epoch())
          .count();
  span_ptr->setTimestamp(timestamp_micro);

  span_ptr->setTracer(this);

  return span_ptr;
}

SpanPtr Tracer::startSpan(const std::string &span_name, SystemTime timestamp,
                          const SpanContext &previous_context) {
  SpanPtr span_ptr(new Span());
  Annotation annotation;
  uint64_t timestamp_micro;

  timestamp_micro = std::chrono::duration_cast<std::chrono::microseconds>(
                        timestamp.time_since_epoch())
                        .count();

  if (previous_context.annotationSet().sr_ &&
      !previous_context.annotationSet().cs_) {
    // We need to create a new span that is a child of the previous span; no
    // shared context

    // Create a new span id
    uint64_t random_number = RandomUtil::generateId();
    span_ptr->setId(random_number);

    span_ptr->setName(span_name);

    // Set the parent id to the id of the previous span
    span_ptr->setParentId(TraceId(previous_context.id()));

    // Set the CS annotation value
    annotation.setValue(ZipkinCoreConstants::get().CLIENT_SEND);

    // Set the timestamp globally for the span
    span_ptr->setTimestamp(timestamp_micro);
  } else if (previous_context.annotationSet().cs_ &&
             !previous_context.annotationSet().sr_) {
    // We need to create a new span that will share context with the previous
    // span

    // Initialize the shared context for the new span
    span_ptr->setId(previous_context.id());
    if (!previous_context.parent_id().empty()) {
      span_ptr->setParentId(previous_context.parent_id());
    }

    // Set the SR annotation value
    annotation.setValue(ZipkinCoreConstants::get().SERVER_RECV);
  } else {
    return span_ptr; // return an empty span
  }

  // Build the endpoint
  Endpoint ep(service_name_, address_);

  // Add the newly-created annotation to the span
  annotation.setEndpoint(std::move(ep));
  annotation.setTimestamp(timestamp_micro);
  span_ptr->addAnnotation(std::move(annotation));

  // Keep the same trace id
  span_ptr->setTraceId(previous_context.trace_id());

  int64_t start_time_micro =
      std::chrono::duration_cast<std::chrono::microseconds>(
          SteadyClock::now().time_since_epoch())
          .count();
  span_ptr->setStartTime(start_time_micro);

  span_ptr->setTracer(this);

  return span_ptr;
}

void Tracer::reportSpan(Span &&span) {
  if (reporter_) {
    reporter_->reportSpan(std::move(span));
  }
}

void Tracer::setReporter(ReporterPtr reporter) {
  reporter_ = std::move(reporter);
}
} // namespace zipkin
