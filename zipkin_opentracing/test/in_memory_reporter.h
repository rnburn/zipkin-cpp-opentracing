#pragma once

#include <mutex>
#include <vector>
#include <zipkin/tracer.h>

namespace zipkin {
// InMemoryReporter is used for testing only.
class InMemoryReporter : public Reporter {
public:
  void reportSpan(const Span &span) override {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    spans_.emplace_back(std::move(span));
  }

  std::vector<Span> spans() const {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    return spans_;
  }

  size_t size() const {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    return spans_.size();
  }

  Span top() const {
    std::lock_guard<std::mutex> lock_guard(mutex_);
    if (spans_.empty())
      throw std::runtime_error("no spans");
    return spans_.back();
  }

private:
  mutable std::mutex mutex_;
  std::vector<Span> spans_;
};
} // namespace zipkin
