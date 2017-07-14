#include <cassert>
#include <iostream>
#include <zipkin/hex.h>
#include <zipkin/utility.h>
using namespace zipkin;

int main() {
  assert(Hex::hexToUint64("1").value() == 1);
  assert(Hex::hexToUint64("a").value() == 10);
  assert(Hex::hexToUint64("11").value() == 17);
  assert(Hex::hexToUint64("011").value() == 17);
  assert(Hex::hexToUint64("0011").value() == 17);
  assert(Hex::hexToUint64("100").value() == 256);
  assert(Hex::hexToUint64("1000").value() == 4096);

  // Check against random 64-bit integers.
  for (int i = 0; i < 10; ++i) {
    auto id = RandomUtil::generateId();
    auto s = Hex::uint64ToHex(id);
    auto id_maybe = Hex::hexToUint64(s);
    assert(id_maybe.valid());
    assert(id_maybe.value() == id);

    auto trace_id_maybe = Hex::hexToTraceId(s);
    assert(trace_id_maybe.valid());
    assert(trace_id_maybe.value().low() == id);
    assert(trace_id_maybe.value().high() == 0);
  }

  // Check against random 128-bit Trace IDs.
  for (int i = 0; i < 10; ++i) {
    auto high = RandomUtil::generateId();
    auto low = RandomUtil::generateId();
    auto trace_id = TraceId{high, low};
    auto s = Hex::traceIdToHex(trace_id);
    auto trace_id_maybe = Hex::hexToTraceId(s);
    assert(trace_id_maybe.valid());
    assert(trace_id_maybe.value().low() == low);
    assert(trace_id_maybe.value().high() == high);
  }
  return 0;
}
