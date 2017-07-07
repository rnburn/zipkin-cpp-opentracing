#pragma once

namespace zipkin {

class Span;

/**
 * This interface must be observed by a Zipkin tracer.
 */
class TracerInterface {
public:
  /**
   * Destructor.
   */
  virtual ~TracerInterface() {}

  /**
   * A Zipkin tracer must implement this method. Its implementation must perform
   * whatever
   * actions are required when the given span is considered finished. An
   * implementation
   * will typically buffer the given span so that it can be flushed later.
   *
   * This method is invoked by the Span object when its finish() method is
   * called.
   *
   * @param span The span that needs action.
   */
  virtual void reportSpan(Span &&span) = 0;
};
} // namespace zipkin
