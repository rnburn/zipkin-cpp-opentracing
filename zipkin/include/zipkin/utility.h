#pragma once

#include <chrono>
#include <cstdint>
#include <sstream>
#include <string>
#include <strings.h>
#include <vector>

namespace zipkin {
using SystemClock = std::chrono::system_clock;
using SteadyClock = std::chrono::steady_clock;
using SystemTime = SystemClock::time_point;
using SteadyTime = SteadyClock::time_point;

/**
 * Utility routines for working with random number generation.
 */
class RandomUtil {
public:
  static uint64_t generateId();
};

/**
 * Utility routines for working with strings.
 */
class StringUtil {
public:
  /**
   * Convert a string to an unsigned long, checking for error.
   * @param return TRUE if successful, FALSE otherwise.
   */
  static bool atoul(const char *str, uint64_t &out, int base = 10);

  /**
   * Convert a string to an unsigned long long, checking for error.
   * @param return TRUE if successful, FALSE otherwise.
   */
  static bool atoull(const char *str, uint64_t &out, int base = 10);

  /**
   * Perform a case insensitive compare of 2 strings.
   * @param lhs supplies string 1.
   * @param rhs supplies string 2.
   * @return < 0, 0, > 0 depending on the comparison result.
   */
  static int caseInsensitiveCompare(const char *lhs, const char *rhs) {
    return strcasecmp(lhs, rhs);
  }

  /**
   * Convert an unsigned integer to a base 10 string as fast as possible.
   * @param out supplies the string to fill.
   * @param out_len supplies the length of the output buffer. Must be >= 21.
   * @param i supplies the number to convert.
   * @return the size of the string, not including the null termination.
   */
  static uint32_t itoa(char *out, size_t out_len, uint64_t i);

  /**
   * Trim trailing whitespace from a string in place.
   */
  static void rtrim(std::string &source);

  /**
   * Size-bounded string copying and concatenation
   */
  static size_t strlcpy(char *dst, const char *src, size_t size);

  /**
   * Split a string.
   * @param source supplies the string to split.
   * @param split supplies the string to split on.
   * @param keep_empty_string result contains empty strings if the string starts
   * or ends with
   * 'split', or if instances of 'split' are adjacent.
   * @return vector of strings computed after splitting `source` around all
   * instances of `split`.
   */
  static std::vector<std::string> split(const std::string &source,
                                        const std::string &split,
                                        bool keep_empty_string = false);

  /**
   * Join elements of a vector into a string delimited by delimiter.
   * @param source supplies the strings to join.
   * @param delimiter supplies the delimiter to join them together.
   * @return string combining elements of `source` with `delimiter` in between
   * each element.
   */
  static std::string join(const std::vector<std::string> &source,
                          const std::string &delimiter);

  /**
   * Split a string.
   * @param source supplies the string to split.
   * @param split supplies the char to split on.
   * @return vector of strings computed after splitting `source` around all
   * instances of `split`.
   */
  static std::vector<std::string> split(const std::string &source, char split);

  /**
   * Version of substr() that operates on a start and end index instead of a
   * start index and a
   * length.
   */
  static std::string subspan(const std::string &source, size_t start,
                             size_t end);

  /**
   * Escape strings for logging purposes. Returns a copy of the string with
   * \n, \r, \t, and " (double quote) escaped.
   * @param source supplies the string to escape.
   * @return escaped string.
   */
  static std::string escape(const std::string &source);

  /**
   * @return true if @param source ends with @param end.
   */
  static bool endsWith(const std::string &source, const std::string &end);

  /**
   * @param case_sensitive determines if the compare is case sensitive
   * @return true if @param source starts with @param start and ignores cases.
   */
  static bool startsWith(const char *source, const std::string &start,
                         bool case_sensitive = true);
};

/**
 * Utility routines for working with json.
 */
class JsonUtil {
public:
  // ====
  // Stringified-JSON manipulation
  // ====

  /**
   * Merges the stringified JSONs given in target and source.
   *
   * @param target It will contain the resulting stringified JSON.
   * @param source The stringified JSON that will be added to target.
   * @param field_name The key name (added to target's JSON) whose value will be
   * the JSON in source.
   */
  static void mergeJsons(std::string &target, const std::string &source,
                         const std::string &field_name);

  /**
   * Merges a stringified JSON and a vector of stringified JSONs.
   *
   * @param target It will contain the resulting stringified JSON.
   * @param json_array Vector of strings, where each element is a stringified
   * JSON.
   * @param field_name The key name (added to target's JSON) whose value will be
   * a stringified.
   * JSON array derived from json_array.
   */
  static void addArrayToJson(std::string &target,
                             const std::vector<std::string> &json_array,
                             const std::string &field_name);
};
} // namespace zipkin
