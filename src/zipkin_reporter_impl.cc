#include "zipkin_reporter_impl.h"

namespace zipkin {

const SteadyClock::duration reporting_period = std::chrono::milliseconds{500};
const size_t max_buffered_spans = 5;

ReporterImpl::ReporterImpl(TransporterPtr&& transporter)
    : transporter_{std::move(transporter)},
      spans_{max_buffered_spans},
      inflight_spans_{max_buffered_spans} {
  writer_ = std::thread(&ReporterImpl::writeReports, this);
}

ReporterImpl::~ReporterImpl() {
  makeWriterExit();
  writer_.join();
}

void ReporterImpl::reportSpan(const Span& span) {
  bool isFull;
  {
    std::lock_guard<std::mutex> lock(write_mutex_);
    spans_.addSpan(span);
    isFull = spans_.pendingSpans() == max_buffered_spans;
  }
  if (isFull)
    write_cond_.notify_one();
}

void ReporterImpl::makeWriterExit() {
    std::lock_guard<std::mutex> lock(write_mutex_);
    write_exit_ = true;
    write_cond_.notify_all();
}

bool ReporterImpl::waitUntilNextReport(const SteadyTime& due_time) {
     std::unique_lock<std::mutex> lock{write_mutex_};
     write_cond_.wait_until(lock, due_time,
                            [this] { return this->write_exit_; });
     if (!write_exit_) {
       inflight_spans_.swap(spans_);
     }
     return !write_exit_;
}

void ReporterImpl::writeReports() {
      auto due_time = std::chrono::steady_clock::now() + reporting_period;
      while (waitUntilNextReport(due_time)) {
        transporter_->transportSpans(inflight_spans_);
        inflight_spans_.clear();
        auto now = std::chrono::steady_clock::now();
        due_time += reporting_period;
        if (due_time < now)
          due_time = now;
      }
}
} // namespace zipkin
