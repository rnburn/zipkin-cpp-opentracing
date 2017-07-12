#include <zipkin/hex.h>
#include <zipkin/utility.h>

#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace zipkin {
std::string Hex::encode(const uint8_t *data, size_t length) {
  static const char *const digits = "0123456789abcdef";

  std::string ret;
  ret.reserve(length * 2);

  for (size_t i = 0; i < length; i++) {
    uint8_t d = data[i];
    ret.push_back(digits[d >> 4]);
    ret.push_back(digits[d & 0xf]);
  }

  return ret;
}

static std::vector<uint8_t> decode(const std::string &hex_string) {
  if (hex_string.empty()) {
    return {};
  }
  auto num_bytes = hex_string.size() / 2 + (hex_string.size() % 2 > 0);
  std::vector<uint8_t> result;
  result.reserve(num_bytes);

  std::vector<uint8_t> segment;
  segment.reserve(num_bytes);
  size_t i = 0;
  std::string hex_byte;
  hex_byte.reserve(2);
  uint64_t out;

  if (hex_string.size() % 2 == 1) {
    hex_byte = hex_string.substr(0, 1);
    if (!StringUtil::atoul(hex_byte.c_str(), out, 16)) {
      return {};
    }
    segment.push_back(out);
    i = 1;
  }

  for (; i < hex_string.size(); i += 2) {
    hex_byte = hex_string.substr(i, 2);
    if (!StringUtil::atoul(hex_byte.c_str(), out, 16)) {
      return {};
    }

    segment.push_back(out);
  }

  return segment;
}

std::string Hex::uint64ToHex(uint64_t value) {
  std::array<uint8_t, 8> data;

  // This is explicitly done for performance reasons
  data[7] = (value & 0x00000000000000FF);
  data[6] = (value & 0x000000000000FF00) >> 8;
  data[5] = (value & 0x0000000000FF0000) >> 16;
  data[4] = (value & 0x00000000FF000000) >> 24;
  data[3] = (value & 0x000000FF00000000) >> 32;
  data[2] = (value & 0x0000FF0000000000) >> 40;
  data[1] = (value & 0x00FF000000000000) >> 48;
  data[0] = (value & 0xFF00000000000000) >> 56;

  return encode(&data[0], data.size());
}

std::string Hex::traceIdToHex(const TraceId &trace_id) {
  std::string result;
  if (trace_id.high() == 0) {
    result.reserve(16);
    result = Hex::uint64ToHex(trace_id.low());
  } else {
    result.reserve(32);
    result.append(Hex::uint64ToHex(trace_id.high()));
    result.append(Hex::uint64ToHex(trace_id.low()));
  }
  return result;
}

Optional<uint64_t> Hex::hexToUint64(const std::string &s) {
  auto data = decode(s);
  if (data.empty() || data.size() > 8) {
    return {};
  }
  uint64_t result = 0;
  for (auto d : data) {
    result = result * 256 + d;
  }
  return result;
}

Optional<TraceId> Hex::hexToTraceId(const std::string &s) {
  auto data = decode(s);
  if (data.empty() || data.size() > 16) {
    return {};
  }
  uint64_t trace_id_high = 0;
  uint64_t trace_id_low = 0;
  size_t num_bytes_low = std::min<size_t>(8, data.size());
  size_t num_bytes_high = data.size() - num_bytes_low;
  for (size_t i = 0; i < num_bytes_high; ++i) {
    trace_id_high = trace_id_high * 256 + data[i];
  }
  for (size_t i = num_bytes_high; i < data.size(); ++i) {
    trace_id_low = trace_id_low * 256 + data[i];
  }
  return TraceId{trace_id_high, trace_id_low};
}
} // namespace zipkin
