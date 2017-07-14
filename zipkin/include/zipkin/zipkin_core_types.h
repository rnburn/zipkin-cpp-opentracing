#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <vector>

#include <zipkin/hex.h>
#include <zipkin/ip_address.h>
#include <zipkin/optional.h>
#include <zipkin/trace_id.h>
#include <zipkin/tracer_interface.h>

namespace zipkin {

/**
 * Base class to be inherited by all classes that represent Zipkin-related
 * concepts, namely:
 * endpoint, annotation, binary annotation, and span.
 */
class ZipkinBase {
public:
  /**
   * Destructor.
   */
  virtual ~ZipkinBase() {}

  /**
   * All classes defining Zipkin abstractions need to implement this method to
   * convert
   * the corresponding abstraction to a Zipkin-compliant JSON.
   */
  virtual const std::string toJson() = 0;
};

/**
 * Represents a Zipkin endpoint. This class is based on Zipkin's Thrift
 * definition of an endpoint.
 * Endpoints can be added to Zipkin annotations.
 */
class Endpoint : public ZipkinBase {
public:
  /**
   * Default constructor. Creates an empty Endpoint.
   */
  Endpoint() = default;

  /**
   * Constructor that initializes an endpoint with the given attributes.
   *
   * @param service_name String representing the endpoint's service name
   * @param address Pointer to an object representing the endpoint's network
   * address
   */
  Endpoint(const std::string &service_name, const IpAddress &address)
      : service_name_(service_name), address_(address) {}

  /**
   * @return the endpoint's address.
   */
  const IpAddress &address() const { return address_; }

  /**
   * Sets the endpoint's address
   */
  void setAddress(const IpAddress &address) { address_ = address; }

  /**
   * @return the endpoint's service name attribute.
   */
  const std::string &serviceName() const { return service_name_; }

  /**
   * Sets the endpoint's service name attribute.
   */
  void setServiceName(const std::string &service_name) {
    service_name_ = service_name;
  }

  /**
   * Serializes the endpoint as a Zipkin-compliant JSON representation as a
   * string.
   *
   * @return a stringified JSON.
   */
  const std::string toJson() override;

private:
  std::string service_name_;
  IpAddress address_;
};

/**
 * Represents a Zipkin basic annotation. This class is based on Zipkin's Thrift
 * definition of
 * an annotation.
 */
class Annotation : public ZipkinBase {
public:
  /**
   * Copy constructor.
   */
  Annotation(const Annotation &);

  /**
   * Assignment operator.
   */
  Annotation &operator=(const Annotation &);

  /**
   * Default constructor. Creates an empty annotation.
   */
  Annotation() : timestamp_(0), value_() {}

  /**
   * Constructor that creates an annotation based on the given parameters.
   *
   * @param timestamp A 64-bit integer containing the annotation timestasmp
   * attribute.
   * @param value A string containing the annotation's value attribute. Valid
   * values appear on ZipkinCoreConstants. The most commonly used values are
   * "cs", "cr", "ss" and "sr".
   * @param endpoint The endpoint object representing the annotation's endpoint
   * attribute.
   */
  Annotation(uint64_t timestamp, const std::string value, Endpoint &endpoint)
      : timestamp_(timestamp), value_(value), endpoint_(endpoint) {}

  /**
   * @return the annotation's endpoint attribute.
   */
  const Endpoint &endpoint() const { return endpoint_.value(); }

  /**
   * Sets the annotation's endpoint attribute (copy semantics).
   */
  void setEndpoint(const Endpoint &endpoint) { endpoint_.value(endpoint); }

  /**
   * Sets the annotation's endpoint attribute (move semantics).
   */
  void setEndpoint(const Endpoint &&endpoint) { endpoint_.value(endpoint); }

  /**
   * Replaces the endpoint's service-name attribute value with the given value.
   *
   * @param service_name String with the new service name.
   */
  void changeEndpointServiceName(const std::string &service_name);

  /**
   * @return the annotation's timestamp attribute
   * (clock time for user presentation: microseconds since epoch).
   */
  uint64_t timestamp() const { return timestamp_; }

  /**
   * Sets the annotation's timestamp attribute.
   */
  void setTimestamp(uint64_t timestamp) { timestamp_ = timestamp; }

  /**
   * return the annotation's value attribute.
   */
  const std::string &value() const { return value_; }

  /**
   * Sets the annotation's value attribute.
   */
  void setValue(const std::string &value) { value_ = value; }

  /**
   * @return true if the endpoint attribute is set, or false otherwise.
   */
  bool isSetEndpoint() const { return endpoint_.valid(); }

  /**
   * Serializes the annotation as a Zipkin-compliant JSON representation as a
   * string.
   *
   * @return a stringified JSON.
   */
  const std::string toJson() override;

private:
  uint64_t timestamp_;
  std::string value_;
  Optional<Endpoint> endpoint_;
};

/**
 * Enum representing valid types of Zipkin binary annotations.
 */
enum AnnotationType { BOOL = 0, STRING = 1, INT64 = 2, DOUBLE = 3 };

/**
 * Represents a Zipkin binary annotation. This class is based on Zipkin's Thrift
 * definition of a binary annotation. A binary annotation allows arbitrary
 * key-value pairs to be associated with a Zipkin span.
 */
class BinaryAnnotation : public ZipkinBase {
public:
  /**
   * Default constructor. Creates an empty binary annotation.
   */
  BinaryAnnotation() : key_(), value_string_(), annotation_type_(STRING) {}

  /**
   * Constructor that creates a binary annotation based on the given parameters.
   *
   * @param key The key name of the annotation.
   * @param value The value associated with the key.
   */
  BinaryAnnotation(const std::string &key, const std::string &value)
      : key_(key), value_string_(value), annotation_type_(STRING) {}

  /**
   * @return the type of the binary annotation.
   */
  AnnotationType annotationType() const { return annotation_type_; }

  /**
   * @return the annotation's endpoint attribute.
   */
  const Endpoint &endpoint() const { return endpoint_.value(); }

  /**
   * Sets the annotation's endpoint attribute (copy semantics).
   */
  void setEndpoint(const Endpoint &endpoint) { endpoint_.value(endpoint); }

  /**
   * Sets the annotation's endpoint attribute (move semantics).
   */
  void setEndpoint(const Endpoint &&endpoint) { endpoint_.value(endpoint); }

  /**
   * @return true of the endpoint attribute has been set, or false otherwise.
   */
  bool isSetEndpoint() const { return endpoint_.valid(); }

  /**
   * @return the key attribute.
   */
  const std::string &key() const { return key_; }

  /**
   * Sets the key attribute.
   */
  void setKey(const std::string &key) { key_ = key; }

  /**
   * @return the value attribute.
   */
  const std::string &valueString() const {
    assert(annotation_type_ == STRING);
    return value_string_;
  }

  bool valueBool() const {
    assert(annotation_type_ == BOOL);
    return value_bool_;
  }

  int64_t valueInt64() const {
    assert(annotation_type_ == INT64);
    return value_int64_;
  }

  double valueDouble() const {
    assert(annotation_type_ == DOUBLE);
    return value_double_;
  }

  /**
   * Sets the value attribute.
   */
  void setValue(const std::string &value) {
    annotation_type_ = STRING;
    value_string_ = value;
  }

  void setValue(bool value) {
    annotation_type_ = BOOL;
    value_bool_ = value;
  }

  void setValue(int64_t value) {
    annotation_type_ = INT64;
    value_int64_ = value;
  }

  void setValue(double value) {
    annotation_type_ = DOUBLE;
    value_double_ = value;
  }

  /**
   * Serializes the binary annotation as a Zipkin-compliant JSON representation
   * as a string.
   *
   * @return a stringified JSON.
   */
  const std::string toJson() override;

private:
  std::string key_;
  std::string value_string_;
  union {
    bool value_bool_;
    int64_t value_int64_;
    double value_double_;
  };
  Optional<Endpoint> endpoint_;
  AnnotationType annotation_type_;
};

typedef std::unique_ptr<Span> SpanPtr;

/**
 * Represents a Zipkin span. This class is based on Zipkin's Thrift definition
 * of a span.
 */
class Span : public ZipkinBase {
public:
  /**
   * Default constructor. Creates an empty span.
   */
  Span()
      : trace_id_(0), name_(), id_(0), debug_(false), monotonic_start_time_(0),
        tracer_(nullptr) {}

  /**
   * Sets the span's trace id attribute.
   */
  void setTraceId(const TraceId val) { trace_id_ = val; }

  /**
   * Sets the span's name attribute.
   */
  void setName(const std::string &val) { name_ = val; }

  /**
   * Sets the span's id.
   */
  void setId(const uint64_t val) { id_ = val; }

  /**
   * Sets the span's parent id.
   */
  void setParentId(const TraceId val) { parent_id_.value(val); }

  /**
   * @return Whether or not the parent_id attribute is set.
   */
  bool isSetParentId() const { return parent_id_.valid(); }

  /**
   * @return a vector with all annotations added to the span.
   */
  const std::vector<Annotation> &annotations() { return annotations_; }

  /**
   * Sets the span's annotations all at once.
   */
  void setAnnotations(const std::vector<Annotation> &val) {
    annotations_ = val;
  }

  /**
   * Adds an annotation to the span (copy semantics).
   */
  void addAnnotation(const Annotation &ann) { annotations_.push_back(ann); }

  /**
   * Adds an annotation to the span (move semantics).
   */
  void addAnnotation(const Annotation &&ann) { annotations_.push_back(ann); }

  /**
   * Sets the span's binary annotations all at once.
   */
  void setBinaryAnnotations(const std::vector<BinaryAnnotation> &val) {
    binary_annotations_ = val;
  }

  /**
   * Adds a binary annotation to the span (copy semantics).
   */
  void addBinaryAnnotation(const BinaryAnnotation &bann) {
    binary_annotations_.push_back(bann);
  }

  /**
   * Adds a binary annotation to the span (move semantics).
   */
  void addBinaryAnnotation(const BinaryAnnotation &&bann) {
    binary_annotations_.push_back(bann);
  }

  /**
   * Sets the span's debug attribute.
   */
  void setDebug() { debug_ = true; }

  /**
   * Sets the span's timestamp attribute.
   */
  void setTimestamp(const int64_t val) { timestamp_.value(val); }

  /**
   * @return Whether or not the timestamp attribute is set.
   */
  bool isSetTimestamp() const { return timestamp_.valid(); }

  /**
   * Sets the span's duration attribute.
   */
  void setDuration(const int64_t val) { duration_.value(val); }

  /**
   * @return Whether or not the duration attribute is set.
   */
  bool isSetDuration() const { return duration_.valid(); }

  /**
   * Sets the span start-time attribute (monotonic, used to calculate duration).
   */
  void setStartTime(const int64_t time) { monotonic_start_time_ = time; }

  /**
   * @return the span's annotations.
   */
  const std::vector<Annotation> &annotations() const { return annotations_; }

  /**
   * @return the span's binary annotations.
   */
  const std::vector<BinaryAnnotation> &binaryAnnotations() const {
    return binary_annotations_;
  }

  /**
   * @return the span's duration attribute.
   */
  int64_t duration() const { return duration_.value(); }

  /**
   * @return the span's id as an integer.
   */
  uint64_t id() const { return id_; }

  /**
   * @return the span's id as a hexadecimal string.
   */
  const std::string idAsHexString() const { return Hex::uint64ToHex(id_); }

  /**
   * @return the span's name.
   */
  const std::string &name() const { return name_; }

  /**
   * @return the span's parent id as an integer.
   */
  TraceId parentId() const { return parent_id_.value(); }

  /**
   * @return the span's parent id as a hexadecimal string.
   */
  std::string parentIdAsHexString() const {
    return parent_id_.valid() ? Hex::traceIdToHex(parent_id_.value())
                              : EMPTY_HEX_STRING_;
  }

  /**
   * @return whether or not the debug attribute is set
   */
  bool debug() const { return debug_; }

  /**
   * @return the span's timestamp (clock time for user presentation:
   * microseconds since epoch).
   */
  int64_t timestamp() const { return timestamp_.value(); }

  /**
   * @return the span's trace id as an integer.
   */
  TraceId traceId() const { return trace_id_; }

  /**
   * @return the span's trace id as a hexadecimal string.
   */
  std::string traceIdAsHexString() const {
    return Hex::traceIdToHex(trace_id_);
  }

  /**
   * @return the span's start time (monotonic, used to calculate duration).
   */
  int64_t startTime() const { return monotonic_start_time_; }

  /**
   * Replaces the service-name attribute of the span's basic annotations with
   * the provided value.
   *
   * This method will operate on all basic annotations that are part of the span
   * when the call
   * is made.
   *
   * @param service_name String to be used as the new service name for all basic
   * annotations
   */
  void setServiceName(const std::string &service_name);

  /**
    * Serializes the span as a Zipkin-compliant JSON representation as a string.
    * The resulting JSON string can be used as part of an HTTP POST call to
    * send the span to Zipkin.
    *
    * @return a stringified JSON.
    */
  const std::string toJson() override;

  /**
   * Associates a Tracer object with the span. The tracer's reportSpan() method
   * is invoked by the span's finish() method so that the tracer can decide what
   * to do with the span when it is finished.
   *
   * @param tracer Represents the Tracer object to be associated with the span.
   */
  void setTracer(TracerInterface *tracer) { tracer_ = tracer; }

  /**
   * @return the Tracer object associated with the span.
   */
  TracerInterface *tracer() const { return tracer_; }

  /**
   * Marks a successful end of the span. This method will:
   *
   * (1) determine if it needs to add more annotations to the span (e.g., a span
   * containing a CS annotation will need to add a CR annotation) and add them;
   * (2) compute and set the span's duration; and
   * (3) invoke the tracer's reportSpan() method if a tracer has been associated
   * with the span.
   */
  void finish();

  /**
   * Adds a binary annotation to the span.
   *
   * @param name The binary annotation's key.
   * @param value The binary annotation's value.
   */
  void setTag(const std::string &name, const std::string &value);

private:
  static const std::string EMPTY_HEX_STRING_;
  TraceId trace_id_;
  std::string name_;
  uint64_t id_;
  Optional<TraceId> parent_id_;
  bool debug_;
  std::vector<Annotation> annotations_;
  std::vector<BinaryAnnotation> binary_annotations_;
  Optional<int64_t> timestamp_;
  Optional<int64_t> duration_;
  int64_t monotonic_start_time_;
  TracerInterface *tracer_;
};
} // namespace zipkin
