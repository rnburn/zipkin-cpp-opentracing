#include <cassert>
#include <iostream>
#include <zipkin/hex.h>
#include <zipkin/utility.h>

#define CATCH_CONFIG_MAIN
#include <zipkin/catch/catch.hpp>
using namespace zipkin;

TEST_CASE("hex") {
  CHECK(Hex::hexToUint64("1").value() == 1);
  CHECK(Hex::hexToUint64("a").value() == 10);
  CHECK(Hex::hexToUint64("11").value() == 17);
  CHECK(Hex::hexToUint64("011").value() == 17);
  CHECK(Hex::hexToUint64("0011").value() == 17);
  CHECK(Hex::hexToUint64("100").value() == 256);
  CHECK(Hex::hexToUint64("1000").value() == 4096);

  // Check against random 64-bit integers.
  for (int i = 0; i < 10; ++i) {
    auto id = RandomUtil::generateId();
    auto s = Hex::uint64ToHex(id);
    auto id_maybe = Hex::hexToUint64(s);
    CHECK(id_maybe.valid());
    CHECK(id_maybe.value() == id);

    auto trace_id_maybe = Hex::hexToTraceId(s);
    CHECK(trace_id_maybe.valid());
    CHECK(trace_id_maybe.value().low() == id);
    CHECK(trace_id_maybe.value().high() == 0);
  }

  // Check against random 128-bit Trace IDs.
  for (int i = 0; i < 10; ++i) {
    auto high = RandomUtil::generateId();
    auto low = RandomUtil::generateId();
    auto trace_id = TraceId{high, low};
    auto s = Hex::traceIdToHex(trace_id);
    auto trace_id_maybe = Hex::hexToTraceId(s);
    CHECK(trace_id_maybe.valid());
    CHECK(trace_id_maybe.value().low() == low);
    CHECK(trace_id_maybe.value().high() == high);
  }
}
