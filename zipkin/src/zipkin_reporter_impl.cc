#include "zipkin_reporter_impl.h"
#include <iostream>

namespace zipkin {

const SteadyClock::duration reporting_period = std::chrono::milliseconds{500};
const size_t max_buffered_spans = 1000;

ReporterImpl::ReporterImpl(TransporterPtr &&transporter)
    : transporter_{std::move(transporter)}, spans_{max_buffered_spans},
      inflight_spans_{max_buffered_spans} {
  writer_ = std::thread(&ReporterImpl::writeReports, this);
}

ReporterImpl::~ReporterImpl() {
  makeWriterExit();
  writer_.join();
}

void ReporterImpl::reportSpan(const Span &span) {
  bool is_full;
  {
    std::lock_guard<std::mutex> lock(write_mutex_);
    num_spans_reported_ += spans_.addSpan(span);
    is_full = spans_.pendingSpans() == max_buffered_spans;
  }
  if (is_full)
    write_cond_.notify_one();
}

bool ReporterImpl::flushWithTimeout(
    std::chrono::system_clock::duration timeout) {
  // Note: there is no effort made to speed up the flush when
  // requested, it simply waits for the regularly scheduled flush
  // operations to clear out all the presently pending data.
  std::unique_lock<std::mutex> lock{write_mutex_};

  auto num_spans_snapshot = num_spans_reported_;
  return write_cond_.wait_for(lock, timeout, [this, num_spans_snapshot] {
    return this->num_spans_flushed_ >= num_spans_snapshot;
  });
}

void ReporterImpl::makeWriterExit() {
  std::lock_guard<std::mutex> lock(write_mutex_);
  write_exit_ = true;
  write_cond_.notify_all();
}

bool ReporterImpl::waitUntilNextReport(const SteadyTime &due_time) {
  std::unique_lock<std::mutex> lock{write_mutex_};
  write_cond_.wait_until(lock, due_time, [this] { return this->write_exit_; });
  if (!write_exit_) {
    inflight_spans_.swap(spans_);
  }
  return !write_exit_;
}

void ReporterImpl::writeReports() {
  auto due_time = std::chrono::steady_clock::now() + reporting_period;
  while (waitUntilNextReport(due_time)) {
    if (inflight_spans_.pendingSpans() > 0) {
      transporter_->transportSpans(inflight_spans_);
      num_spans_flushed_ += inflight_spans_.pendingSpans();
      inflight_spans_.clear();
      write_cond_.notify_all();
    }
    auto now = std::chrono::steady_clock::now();
    due_time += reporting_period;
    if (due_time < now)
      due_time = now;
  }
}
} // namespace zipkin
