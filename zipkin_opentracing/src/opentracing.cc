#include <zipkin/opentracing.h>

#include "propagation.h"
#include "utility.h"
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <zipkin/tracer.h>
#include <zipkin/utility.h>
#include <zipkin/zipkin_core_types.h>

using opentracing::string_view;
using opentracing::Value;
using opentracing::expected;
using opentracing::make_unexpected;

namespace ot = opentracing;

namespace zipkin {
static std::tuple<SystemTime, SteadyTime>
computeStartTimestamps(const SystemTime &start_system_timestamp,
                       const SteadyTime &start_steady_timestamp) {
  // If neither the system nor steady timestamps are set, get the tme from the
  // respective clocks; otherwise, use the set timestamp to initialize the
  // other.
  if (start_system_timestamp == SystemTime() &&
      start_steady_timestamp == SteadyTime()) {
    return {SystemClock::now(), SteadyClock::now()};
  }
  if (start_system_timestamp == SystemTime()) {
    return {ot::convert_time_point<SystemClock>(start_steady_timestamp),
            start_steady_timestamp};
  }
  if (start_steady_timestamp == SteadyTime()) {
    return {start_system_timestamp,
            ot::convert_time_point<SteadyClock>(start_system_timestamp)};
  }
  return {start_system_timestamp, start_steady_timestamp};
}

class OtSpanContext : public ot::SpanContext {
public:
  OtSpanContext() = default;

  explicit OtSpanContext(zipkin::SpanContext &&span_context)
      : span_context_{std::move(span_context)} {}

  OtSpanContext(zipkin::SpanContext &&span_context,
                std::unordered_map<std::string, std::string> &&baggage)
      : span_context_{std::move(span_context)}, baggage_{std::move(baggage)} {}

  OtSpanContext(OtSpanContext &&other) {
    span_context_ = std::move(other.span_context_);
    baggage_ = std::move(other.baggage_);
  }

  OtSpanContext &operator=(OtSpanContext &&other) {
    std::lock_guard<std::mutex> lock_guard{baggage_mutex_};
    span_context_ = std::move(other.span_context_);
    baggage_ = std::move(other.baggage_);
    return *this;
  }

  void ForeachBaggageItem(
      std::function<bool(const std::string &, const std::string &)> f)
      const override {}

  template <class Carrier> expected<void> Inject(Carrier &writer) const {
    std::lock_guard<std::mutex> lock_guard{baggage_mutex_};
    return injectSpanContext(writer, span_context_, baggage_);
  }

private:
  zipkin::SpanContext span_context_;
  mutable std::mutex baggage_mutex_;
  std::unordered_map<std::string, std::string> baggage_;

  friend class OtSpan;
};

static const OtSpanContext *findSpanContext(
    const std::vector<std::pair<ot::SpanReferenceType, const ot::SpanContext *>>
        &references) {
  for (auto &reference : references) {
    if (auto span_context =
            dynamic_cast<const OtSpanContext *>(reference.second)) {
      return span_context;
    }
  }
  return nullptr;
}

class OtSpan : public ot::Span {
public:
  OtSpan(std::shared_ptr<const ot::Tracer> &&tracer_owner, SpanPtr &&span_owner,
         const ot::StartSpanOptions &options)
      : tracer_{std::move(tracer_owner)}, span_{std::move(span_owner)} {
    // Set IDs.
    span_->setId(RandomUtil::generateId());
    if (auto parent_span_context = findSpanContext(options.references)) {
      span_->setTraceId(parent_span_context->span_context_.trace_id());
      span_->setParentId(parent_span_context->span_context_.id());
    } else {
      span_->setTraceId(RandomUtil::generateId());
    }

    // Set timestamp.
    SystemTime start_system_timestamp;
    std::tie(start_system_timestamp, start_steady_timestamp_) =
        computeStartTimestamps(options.start_system_timestamp,
                               options.start_steady_timestamp);
    span_->setTimestamp(std::chrono::duration_cast<std::chrono::microseconds>(
                            start_system_timestamp.time_since_epoch())
                            .count());

    // Set tags.
    for (auto &tag : options.tags) {
      tags_[tag.first] = tag.second;
    }

    // Set context.
    span_context_ = OtSpanContext{zipkin::SpanContext{*span_}};
  }

  ~OtSpan() override {
    if (!is_finished_)
      this->Finish();
  }

  void FinishWithOptions(const ot::FinishSpanOptions &options) noexcept override
      try {
    // Ensure the span is only finished once.
    if (is_finished_.exchange(true)) {
      return;
    }

    // Set timing information.
    auto finish_timestamp = options.finish_steady_timestamp;
    if (finish_timestamp == SteadyTime()) {
      finish_timestamp = SteadyClock::now();
    }
    auto duration = finish_timestamp - start_steady_timestamp_;
    span_->setDuration(
        std::chrono::duration_cast<std::chrono::microseconds>(duration)
            .count());

    // Set tags and finish
    std::lock_guard<std::mutex> lock{mutex_};
    for (const auto &tag : tags_) {
      span_->addBinaryAnnotation(toBinaryAnnotation(tag.first, tag.second));
    }
    span_->finish();
  } catch (const std::bad_alloc &) {
    // Do nothing if memory allocation fails.
  }

  void SetOperationName(string_view name) noexcept override try {
    std::lock_guard<std::mutex> lock_guard{mutex_};
    span_->setName(name);
  } catch (const std::bad_alloc &) {
    // Do nothing if memory allocation fails.
  }

  void SetTag(string_view key, const Value &value) noexcept override try {
    std::lock_guard<std::mutex> lock_guard{mutex_};
    tags_[key] = value;
  } catch (const std::bad_alloc &) {
    // Do nothing if memory allocation fails.
  }

  void SetBaggageItem(string_view restricted_key,
                      string_view value) noexcept override {}

  std::string BaggageItem(string_view restricted_key) const noexcept override {
    return {};
  }

  void Log(std::initializer_list<std::pair<string_view, Value>>
               fields) noexcept override {}

  const ot::SpanContext &context() const noexcept override {
    return span_context_;
  }

  const ot::Tracer &tracer() const noexcept override { return *tracer_; }

private:
  std::shared_ptr<const ot::Tracer> tracer_;
  OtSpanContext span_context_;
  SteadyTime start_steady_timestamp_;

  // Mutex protects tags_ and span_
  std::atomic<bool> is_finished_{false};
  std::mutex mutex_;
  std::unordered_map<std::string, Value> tags_;
  SpanPtr span_;
};

class OtTracer : public ot::Tracer,
                 public std::enable_shared_from_this<OtTracer> {
public:
  explicit OtTracer(TracerPtr &&tracer) : tracer_{std::move(tracer)} {}

  std::unique_ptr<ot::Span>
  StartSpanWithOptions(string_view operation_name,
                       const ot::StartSpanOptions &options) const
      noexcept override {
    // Create the core zipkin span.
    SpanPtr span{new zipkin::Span{}};
    span->setName(operation_name);
    span->setTracer(tracer_.get());

    // Add a binary annotation for the serviceName.
    BinaryAnnotation service_name_annotation{"lc", tracer_->serviceName()};
    service_name_annotation.setEndpoint(
        Endpoint{tracer_->serviceName(), tracer_->address()});
    span->addBinaryAnnotation(std::move(service_name_annotation));

    return std::unique_ptr<ot::Span>{
        new OtSpan{shared_from_this(), std::move(span), options}};
  }

  expected<void> Inject(const ot::SpanContext &sc,
                        std::ostream &writer) const override {
    return InjectImpl(sc, writer);
  }

  expected<void> Inject(const ot::SpanContext &sc,
                        const ot::TextMapWriter &writer) const override {
    return InjectImpl(sc, writer);
  }

  expected<void> Inject(const ot::SpanContext &sc,
                        const ot::HTTPHeadersWriter &writer) const override {
    return InjectImpl(sc, writer);
  }

  expected<std::unique_ptr<ot::SpanContext>>
  Extract(std::istream &reader) const override {
    return ExtractImpl(reader);
  }

  expected<std::unique_ptr<ot::SpanContext>>
  Extract(const ot::TextMapReader &reader) const override {
    return ExtractImpl(reader);
  }

  expected<std::unique_ptr<ot::SpanContext>>
  Extract(const ot::HTTPHeadersReader &reader) const override {
    return ExtractImpl(reader);
  }

  void Close() noexcept override {}

private:
  TracerPtr tracer_;

  template <class Carrier>
  expected<void> InjectImpl(const ot::SpanContext &sc, Carrier &writer) const
      try {
    auto ot_span_context = dynamic_cast<const OtSpanContext *>(&sc);
    if (ot_span_context == nullptr) {
      return make_unexpected(ot::invalid_span_context_error);
    }
    return ot_span_context->Inject(writer);
  } catch (const std::bad_alloc &) {
    return ot::make_unexpected(
        std::make_error_code(std::errc::not_enough_memory));
  }

  template <class Carrier>
  expected<std::unique_ptr<ot::SpanContext>> ExtractImpl(Carrier &reader) const
      try {
    std::unordered_map<std::string, std::string> baggage;
    auto zipkin_span_context_maybe = extractSpanContext(reader, baggage);
    if (!zipkin_span_context_maybe) {
      return ot::make_unexpected(zipkin_span_context_maybe.error());
    }
    if (!zipkin_span_context_maybe->valid()) {
      return std::unique_ptr<ot::SpanContext>{};
    }
    std::unique_ptr<ot::SpanContext> span_context{new OtSpanContext(
        std::move(zipkin_span_context_maybe->value()), std::move(baggage))};
    return std::move(span_context);
  } catch (const std::bad_alloc &) {
    return ot::make_unexpected(
        std::make_error_code(std::errc::not_enough_memory));
  }
};

std::shared_ptr<ot::Tracer>
makeZipkinOtTracer(const ZipkinOtTracerOptions &options) {
  auto reporter =
      makeHttpReporter(options.collector_host.c_str(), options.collector_port);
  TracerPtr tracer{new Tracer{options.service_name, options.service_address}};
  tracer->setReporter(std::move(reporter));
  return std::shared_ptr<ot::Tracer>{new OtTracer{std::move(tracer)}};
}
} // namespace zipkin
