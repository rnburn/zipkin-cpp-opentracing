#pragma once

#include <cassert>

namespace zipkin {
/**
 * Contains an optional value. Like boost::optional and std::optional (not
 * included in C++11).
 */
template <typename T> class Optional {
public:
  Optional() {}
  Optional(const T &value) : value_(value), valid_(true) {}

  bool operator==(const Optional<T> &rhs) const {
    if (valid_) {
      return valid_ == rhs.valid_ && value_ == rhs.value_;
    } else {
      return valid_ == rhs.valid_;
    }
  }

  /**
   * @return whether the contained value is valid.
   */
  bool valid() const { return valid_; }

  /**
   * Set the contained value which will make it valid.
   */
  void value(const T &new_value) {
    value_ = new_value;
    valid_ = true;
  }

  /**
   * @return the contained value.
   */
  const T &value() const {
    assert(valid_);

    return value_;
  }

  /**
   * @return the contained value.
   */
  T &value() {
    assert(valid_);

    return value_;
  }

private:
  T value_;
  bool valid_{};
};
} // namespace zipkin
