#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include "tracer.h"
#include "transporter.h"

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
  explicit ReporterImpl(TransporterPtr &&transporter);

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

private:
  TransporterPtr transporter_;

  std::mutex write_mutex_;
  std::condition_variable write_cond_;
  bool write_exit_ = false;
  std::thread writer_;
  SpanBuffer spans_;
  SpanBuffer inflight_spans_;

  void makeWriterExit();
  bool waitUntilNextReport(const SteadyTime &due_time);
  void writeReports();
};
} // namespace zipkin
