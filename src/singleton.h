#pragma once

namespace zipkin {
/**
 * Immutable singleton pattern.
 */
template <class T> class ConstSingleton {
public:
  /**
   * Obtain an instance of the singleton for class T.
   * @return const T& a reference to the singleton for class T.
   */
  static const T &get() {
    static T *instance = new T();
    return *instance;
  }
};
} // namespace zipkin
