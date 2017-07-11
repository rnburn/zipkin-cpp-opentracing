#pragma once

#include <regex>

#include <zipkin/flags.h>
#include <zipkin/hex.h>
#include <zipkin/optional.h>
#include <zipkin/trace_id.h>
#include <zipkin/zipkin_core_types.h>

namespace zipkin {

/**
 * This struct identifies which Zipkin annotations are present in the
 * span context (see SpanContext)
 * Each member is a one-bit boolean indicating whether or not the
 * corresponding annotation is present.
 *
 * In particular, the following annotations are tracked by this struct:
 * CS: "Client Send"
 * CR: "Client Receive"
 * SS: "Server Send"
 * SR: "Server Receive"
 */
struct AnnotationSet {
  AnnotationSet() : cs_(false), cr_(false), ss_(false), sr_(false) {}
  bool cs_ : 1;
  bool cr_ : 1;
  bool ss_ : 1;
  bool sr_ : 1;
};

/**
 * This class represents the context of a Zipkin span. It embodies the following
 * span characteristics: trace id, span id, parent id, and basic annotations.
 */
class SpanContext {
public:
  /**
   * Default constructor. Creates an empty context.
   */
  SpanContext()
      : trace_id_(0), id_(0), parent_id_(), flags_(0), is_initialized_(false) {}

  /**
   * Constructor that creates a context object from the given Zipkin span
   * object.
   *
   * @param span The Zipkin span used to initialize a SpanContext object.
   */
  SpanContext(const Span &span);

  /**
   * Constructor that creates a context object from the given IDs.
   */
  SpanContext(const TraceId &trace_id, uint64_t id,
              const Optional<TraceId> &parent_id, flags_t flags)
      : trace_id_{trace_id}, id_{id}, parent_id_{parent_id}, flags_{flags},
        is_initialized_{true} {}

  /**
   * @return the span id as an integer
   */
  uint64_t id() const { return id_; }

  /**
   * @return the span id as a 16-character hexadecimal string.
   */
  std::string idAsHexString() const { return Hex::uint64ToHex(id_); }

  /**
   * @return Whether or not the parent_id attribute is set.
   */
  bool isSetParentId() const { return parent_id_.valid(); }

  /**
   * @return the span's parent id as an integer.
   */
  TraceId parent_id() const { return parent_id_.value(); }

  /**
   * @return the parent id as a 16-character hexadecimal string.
   */
  std::string parentIdAsHexString() const {
    return Hex::traceIdToHex(parent_id_.value());
  }

  /**
   * @return the trace id as an integer.
   */
  TraceId trace_id() const { return trace_id_; }

  /**
   * @return the trace id as a 16-character hexadecimal string.
   */
  std::string traceIdAsHexString() const {
    return Hex::traceIdToHex(trace_id_);
  }

  /**
   * @return the flags as an integer.
   */
  flags_t flags() const { return flags_; }

  /**
   * @return a struct indicating which annotations are present in the span.
   */
  AnnotationSet annotationSet() const { return annotation_values_; }

private:
  TraceId trace_id_;
  uint64_t id_;
  Optional<TraceId> parent_id_;
  AnnotationSet annotation_values_;
  flags_t flags_;
  bool is_initialized_;
};
} // namespace zipkin
