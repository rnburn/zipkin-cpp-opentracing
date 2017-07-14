#pragma once

#include <string>
#include <vector>
#include <zipkin/optional.h>
#include <zipkin/trace_id.h>

namespace zipkin {
/**
 * Hex encoder/decoder. Produces lowercase hex digits. Can consume either
 * lowercase or uppercase digits.
 */
class Hex final {
public:
  /**
   * Generates a hex dump of the given data
   * @param data the binary data to convert
   * @return the hex encoded string representing data
   */
  static std::string encode(const std::vector<uint8_t> &data) {
    return encode(&data[0], data.size());
  }

  /**
   * Generates a hex dump of the given data
   * @param data the binary data to convert
   * @param length the length of the data
   * @return the hex encoded string representing data
   */
  static std::string encode(const uint8_t *data, size_t length);

  /**
   * Converts a hex dump to binary data
   * @param input the hex dump to decode
   * @return binary data
   */
  /* static std::vector<uint8_t> decode(const std::string& input); */

  /**
   * Converts the given 64-bit integer into a hexadecimal string.
   * @param value The integer to be converted.
   */
  static std::string uint64ToHex(uint64_t value);

  /**
   * Converts the given TraceId into a hexadecimal string.
   * @param trace_id The TraceId to be converted.
   */
  static std::string traceIdToHex(const TraceId &value);

  /**
   * Converts the given hexadecimal string into a 64-bit integer.
   * @param value The hexadecimal string to be converted.
   */
  static Optional<uint64_t> hexToUint64(const std::string &s);

  /**
   * Converts the given hexadecimal string into a TraceId.
   * @param value The hexadecimal string to be converted.
   */
  static Optional<TraceId> hexToTraceId(const std::string &s);
};
} // namespace zipkin
