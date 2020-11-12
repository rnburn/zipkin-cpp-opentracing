#pragma once

#include <cstdint>

namespace zipkin {
class TraceId {
public:
  TraceId() : high_{0}, low_{0} {}

  TraceId(uint64_t high, uint64_t low) : high_{high}, low_{low} {}

  explicit TraceId(uint64_t low) : TraceId{0, low} {}

  uint64_t high() const { return high_; }

  uint64_t low() const { return low_; }

  bool empty() const { return high_ == 0 && low_ == 0; }

private:
  uint64_t high_;
  uint64_t low_;
};

inline bool operator==(const TraceId &lhs, const TraceId &rhs) {
  return lhs.high() == rhs.high() && lhs.low() == rhs.low();
}

inline bool operator!=(const TraceId &lhs, const TraceId &rhs) {
  return !(lhs == rhs);
}
} // namespace zipkin
