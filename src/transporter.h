#pragma once

#include "span_buffer.h"

namespace zipkin {
/**
 * Abstract class that delegates to users of the Tracer class the responsibility
 * of "transporting" Zipkin spans that have ended its life cycle.
 */
class Transporter {
public:
  /**
   * Destructor.
   */
  virtual ~Transporter() {}

  /**
   * Method that a concrete Transporter class must implement to handle finished
   * spans.
   *
   * @param spans The SpanBuffer that needs action.
   */
  virtual void transportSpans(SpanBuffer &spans) = 0;
};

typedef std::unique_ptr<Transporter> TransporterPtr;
} // namespace zipkin
