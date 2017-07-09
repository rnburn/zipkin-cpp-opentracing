#include <iostream>
#include <zipkin/tracer.h>
using namespace zipkin;

int main() {
  Tracer tracer{{}, {}};
  auto span = tracer.startSpan("abc", SystemClock::now());
  std::cout << span->toJson() << "\n";
  return 0;
}
