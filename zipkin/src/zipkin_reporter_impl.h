#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include "transporter.h"
#include <zipkin/tracer.h>

namespace zipkin {
/**
 * This class derives from the abstract zipkin::Reporter. It buffers spans and
 * relies on a Transporter to send spans to Zipkin.
 *
 * Up to `???` will be buffered. Spans are flushed
 * (sent to Zipkin) either when the buffer is full, or when a timer, set to
 * `????`, expires, whichever happens first.
 *
 * The default values for the runtime parameters are 5 spans and 5000ms.
 */
class ReporterImpl : public Reporter {
public:
  /**
   * Constructor.
   *
   * @param transporter The Transporter to be associated with the reporter.
   */
  ReporterImpl(
      TransporterPtr &&transporter,
      SteadyClock::duration reporting_period = DEFAULT_REPORTING_PERIOD,
      size_t max_buffered_spans = DEFAULT_SPAN_BUFFER_SIZE);

  /**
   * Destructor.
   */
  ~ReporterImpl();

  /**
   * Implementation of zipkin::Reporter::reportSpan().
   *
   * Buffers the given span and calls flushSpans() if the buffer is full.
   *
   * @param span The span to be buffered.
   */
  void reportSpan(const Span &span) override;

  bool flushWithTimeout(std::chrono::system_clock::duration timeout) override;

  uint64_t droppedSpanCount() const override;

  uint64_t getAndResetDroppedSpanCount() override;

  /**
   * @brief getReportPeriod Returns the period of time to wait between auto
   * flushes
   * @return Setting for the duration between auto flushes
   */
  SteadyClock::duration reportPeriod() const override {
    return reporting_period_;
  }

  /**
   * @brief setReportPeriod Adjust the period of time to wait between auto
   * flushes
   * @param report_period The new size of the span buffer
   */
  void setReportPeriod(SteadyClock::duration reporting_period) override {
    reporting_period_ = reporting_period;
  }

  /**
   * @brief getBufferSpanCount Returns the number of spans that can be stored
   * @return
   */
  size_t bufferSpanCount() const override { return max_buffered_spans_; }

  /**
   * @brief setBufferSpanCount Adjust the number of spans that can be stored
   * @param span_count The new size of the span buffer
   */
  void setBufferSpanCount(size_t span_count) override {
    max_buffered_spans_ = span_count;
  }

private:
  TransporterPtr transporter_;

  std::atomic<uint64_t> dropped_span_count_{0};
  std::mutex write_mutex_;
  std::condition_variable write_cond_;
  SteadyClock::duration reporting_period_;
  size_t max_buffered_spans_;

  bool write_exit_ = false;
  std::thread writer_;
  int64_t num_spans_reported_ = 0;
  std::atomic<int64_t> num_spans_flushed_{0};
  SpanBuffer spans_;
  SpanBuffer inflight_spans_;

  void makeWriterExit();
  bool waitUntilNextReport(const SteadyTime &due_time);
  void writeReports();
};
} // namespace zipkin
