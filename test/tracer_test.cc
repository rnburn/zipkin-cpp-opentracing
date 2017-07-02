#include "../src/tracer.h"
#include <iostream>
using namespace zipkin;

int main() {
  Tracer tracer{{}, {}};
  auto span = tracer.startSpan("abc", SystemClock::now());
  std::cout << span->toJson() << "\n";
  return 0;
}
