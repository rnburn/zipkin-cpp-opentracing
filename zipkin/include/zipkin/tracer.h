#pragma once

#include <zipkin/span_context.h>
#include <zipkin/tracer_interface.h>
#include <zipkin/utility.h>
#include <zipkin/zipkin_core_types.h>

namespace zipkin {
const SteadyClock::duration DEFAULT_REPORTING_PERIOD =
    std::chrono::milliseconds{500};
const size_t DEFAULT_SPAN_BUFFER_SIZE = 1000;

/**
 * Abstract class that delegates to users of the Tracer class the responsibility
 * of "reporting" a Zipkin span that has ended its life cycle. "Reporting" can
 * mean that the span will be sent to out to Zipkin, or buffered so that it can
 * be sent out later.
 */
class Reporter {
public:
  /**
   * Destructor.
   */
  virtual ~Reporter() {}

  /**
   * Method that a concrete Reporter class must implement to handle finished
   * spans.  For example, a span-buffer management policy could be implemented.
   *
   * @param span The span that needs action.
   */
  virtual void reportSpan(const Span &span) = 0;

  /**
   * Optional method that a concrete Reporter class can implement to flush
   * buffered spans.
   *
   * @param timeout The timeout to use when flushing.
   */
  virtual bool flushWithTimeout(std::chrono::system_clock::duration timeout) {
    return true;
  }

  /**
   * @return the number of spans that have been dropped since this value was
   * last cleared.
   */
  virtual uint64_t droppedSpanCount() const { return 0; }

  /**
   * In addition to getting the drop count, resets the drop count.
   * @return the number of spans that have been dropped since this value was
   * last reset
   */
  virtual uint64_t getAndResetDroppedSpanCount() { return 0; }

  /**
   * @return the period of time to wait between auto flushes
   */
  virtual SteadyClock::duration reportPeriod() const {
    return SteadyClock::duration::zero();
  }

  /**
   * @param report_period the period of time to wait between auto flushes
   */
  virtual void setReportPeriod(SteadyClock::duration report_period){};

  /**
   * @return the number of spans that can be stored in the buffer
   */
  virtual size_t bufferSpanCount() const { return 0; }

  /**
   * @param span_count The new size of the span buffer
   */
  virtual void setBufferSpanCount(size_t span_count){};
};

typedef std::unique_ptr<Reporter> ReporterPtr;

/**
 * Construct a Reporter that sends spans to a Zipkin service via HTTP.
 *
 * @param collector_host The host to use when sending spans to the Zipkin
 * service.
 * @param collector_port The port to use when sending spans to the Zipkin
 * service.
 * @return a Reporter object.
 */
ReporterPtr makeHttpReporter(
    const char *collector_host, uint32_t collector_port,
    SteadyClock::duration reporting_period = DEFAULT_REPORTING_PERIOD,
    size_t max_buffered_spans = DEFAULT_SPAN_BUFFER_SIZE);

/**
 * This class implements the Zipkin tracer. It has methods to create the
 * appropriate Zipkin span type, i.e., root span, child span, or shared-context
 * span.
 *
 * This class allows its users to supply a concrete Reporter class whose
 * reportSpan method
 * is called by its own reportSpan method. By doing so, we have cleanly
 * separated the logic
 * of dealing with finished spans from the span-creation and tracing logic.
 */
class Tracer : public TracerInterface {
public:
  /**
   * Constructor.
   *
   * @param service_name The name of the service where the Tracer is running.
   * This name is used in all annotations' endpoints of the spans created by the
   * Tracer.
   * @param address Pointer to a network-address object. The IP address and port
   * are used in all annotations' endpoints of the spans created by the Tracer.
   */
  Tracer(const std::string &service_name, const IpAddress &address,
         ReporterPtr &&reporter)
      : service_name_(service_name), address_(address),
        reporter_(std::move(reporter)) {}

  /**
   * Creates a "root" Zipkin span.
   *
   * @param span_name Name of the new span.
   * @param start_time The time indicating the beginning of the span.
   */
  SpanPtr startSpan(const std::string &span_name, SystemTime timestamp);

  /**
   * Depending on the given context, creates either a "child" or a
   * "shared-context" Zipkin span.
   *
   * @param span_name Name of the new span.
   * @param start_time The time indicating the beginning of the span.
   * @param previous_context The context of the span preceding the one to be
   * created.
   */
  SpanPtr startSpan(const std::string &span_name, SystemTime timestamp,
                    const SpanContext &previous_context);

  /**
   * TracerInterface::reportSpan.
   */
  void reportSpan(Span &&span) override;

  /**
   * @return the service-name attribute associated with the Tracer.
   */
  const std::string &serviceName() const { return service_name_; }

  /**
   * @return the pointer to the address object associated with the Tracer.
   */
  const IpAddress &address() const { return address_; }

  /**
   * @return the number of spans that have been dropped since this value was
   * last cleared.
   */
  uint64_t droppedSpanCount() const;

  /**
   * In addition to getting the drop count, resets the drop count.
   * @return the number of spans that have been dropped since this value was
   * last reset
   */
  uint64_t getAndResetDroppedSpanCount();

  /**
   * @return the period of time to wait between auto flushes
   */
  SteadyClock::duration reportPeriod() const;

  /**
   * @param report_period the period of time to wait between auto flushes
   */
  void setReportPeriod(SteadyClock::duration report_period);

  /**
   * @return the number of spans that can be stored in the buffer
   */
  size_t bufferSpanCount() const;

  /**
   * @param span_count The new size of the span buffer
   */
  void setBufferSpanCount(size_t span_count);

  /**
   * Optional method that a concrete Reporter class can implement to flush
   * buffered spans.
   *
   * @param timeout The timeout to use when flushing.
   */
  bool flushWithTimeout(std::chrono::system_clock::duration timeout) {
    if (reporter_)
      return reporter_->flushWithTimeout(timeout);
    return false;
  }

private:
  const std::string service_name_;
  IpAddress address_;
  ReporterPtr reporter_;
};

typedef std::unique_ptr<Tracer> TracerPtr;

} // namespace zipkin
