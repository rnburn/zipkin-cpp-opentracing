#include <zipkin/utility.h>

#include <array>
#include <chrono>
#include <climits>
#include <cstdint>
#include <iterator>
#include <random>
#include <string>
#include <vector>

#include <zipkin/rapidjson/document.h>
#include <zipkin/rapidjson/stringbuffer.h>
#include <zipkin/rapidjson/writer.h>

namespace zipkin {
uint64_t RandomUtil::generateId() {
  static thread_local std::mt19937_64 rand_source{std::random_device{}()};
  return rand_source();
}
/* std::string DateFormatter::fromTime(const SystemTime& time) { */
/*   return fromTimeT(std::chrono::system_clock::to_time_t(time)); */
/* } */

/* std::string DateFormatter::fromTimeT(time_t time) { */
/*   tm current_tm; */
/*   gmtime_r(&time, &current_tm); */

/*   std::array<char, 1024> buf; */
/*   strftime(&buf[0], buf.size(), format_string_.c_str(), &current_tm); */
/*   return std::string(&buf[0]); */
/* } */

/* std::string DateFormatter::now() { */
/*   time_t current_time_t; */
/*   time(&current_time_t); */
/*   return fromTimeT(current_time_t); */
/* } */

/* ProdSystemTimeSource ProdSystemTimeSource::instance_; */
/* ProdMonotonicTimeSource ProdMonotonicTimeSource::instance_; */

/* bool DateUtil::timePointValid(SystemTime time_point) { */
/*   return
 * std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch())
 */
/*              .count() != 0; */
/* } */

/* bool DateUtil::timePointValid(MonotonicTime time_point) { */
/*   return
 * std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch())
 */
/*              .count() != 0; */
/* } */

bool StringUtil::atoul(const char *str, uint64_t &out, int base) {
  if (strlen(str) == 0) {
    return false;
  }

  char *end_ptr;
  out = strtoul(str, &end_ptr, base);
  if (*end_ptr != '\0' || (out == ULONG_MAX && errno == ERANGE)) {
    return false;
  } else {
    return true;
  }
}

uint32_t StringUtil::itoa(char *out, size_t buffer_size, uint64_t i) {
  // The maximum size required for an unsigned 64-bit integer is 21 chars
  // (including null).
  if (buffer_size < 21) {
    throw std::invalid_argument("itoa buffer too small");
  }

  char *current = out;
  do {
    *current++ = "0123456789"[i % 10];
    i /= 10;
  } while (i > 0);

  for (uint64_t i = 0, j = current - out - 1; i < j; i++, j--) {
    char c = out[i];
    out[i] = out[j];
    out[j] = c;
  }

  *current = 0;
  return current - out;
}

void StringUtil::rtrim(std::string &source) {
  std::size_t pos = source.find_last_not_of(" \t\f\v\n\r");
  if (pos != std::string::npos) {
    source.erase(pos + 1);
  } else {
    source.clear();
  }
}

size_t StringUtil::strlcpy(char *dst, const char *src, size_t size) {
  strncpy(dst, src, size - 1);
  dst[size - 1] = '\0';
  return strlen(src);
}

std::vector<std::string> StringUtil::split(const std::string &source,
                                           char split) {
  return StringUtil::split(source, std::string{split});
}

std::vector<std::string> StringUtil::split(const std::string &source,
                                           const std::string &split,
                                           bool keep_empty_string) {
  std::vector<std::string> ret;
  size_t last_index = 0;
  size_t next_index;

  if (split.empty()) {
    ret.emplace_back(source);
    return ret;
  }

  do {
    next_index = source.find(split, last_index);
    if (next_index == std::string::npos) {
      next_index = source.size();
    }

    if (next_index != last_index || keep_empty_string) {
      ret.emplace_back(subspan(source, last_index, next_index));
    }

    last_index = next_index + split.size();
  } while (next_index != source.size());

  return ret;
}

std::string StringUtil::join(const std::vector<std::string> &source,
                             const std::string &delimiter) {
  std::ostringstream buf;
  std::copy(source.begin(), source.end(),
            std::ostream_iterator<std::string>(buf, delimiter.c_str()));
  std::string ret = buf.str();
  // copy will always end with an extra delimiter, we remove it here.
  return ret.substr(0, ret.length() - delimiter.length());
}

std::string StringUtil::subspan(const std::string &source, size_t start,
                                size_t end) {
  return source.substr(start, end - start);
}

std::string StringUtil::escape(const std::string &source) {
  std::string ret;

  // Prevent unnecessary allocation by allocating 2x original size.
  ret.reserve(source.length() * 2);
  for (char c : source) {
    switch (c) {
    case '\r':
      ret += "\\r";
      break;
    case '\n':
      ret += "\\n";
      break;
    case '\t':
      ret += "\\t";
      break;
    case '"':
      ret += "\\\"";
      break;
    default:
      ret += c;
      break;
    }
  }

  return ret;
}

/* std::string AccessLogDateTimeFormatter::fromTime(const SystemTime& time) { */
/*   static DateFormatter date_format("%Y-%m-%dT%H:%M:%S"); */

/*   return fmt::format( */
/*       "{}.{:03d}Z", date_format.fromTime(time), */
/*       std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count()
 * % */
/*           1000); */
/* } */

bool StringUtil::endsWith(const std::string &source, const std::string &end) {
  if (source.length() < end.length()) {
    return false;
  }

  size_t start_position = source.length() - end.length();
  return std::equal(source.begin() + start_position, source.end(), end.begin());
}

bool StringUtil::startsWith(const char *source, const std::string &start,
                            bool case_sensitive) {
  if (case_sensitive) {
    return strncmp(source, start.c_str(), start.size()) == 0;
  } else {
    return strncasecmp(source, start.c_str(), start.size()) == 0;
  }
}

void JsonUtil::mergeJsons(std::string &target, const std::string &source,
                          const std::string &field_name) {
  rapidjson::Document target_doc, source_doc;
  target_doc.Parse(target.c_str());
  source_doc.Parse(source.c_str());

  target_doc.AddMember(rapidjson::StringRef(field_name.c_str()), source_doc,
                       target_doc.GetAllocator());

  rapidjson::StringBuffer sb;
  rapidjson::Writer<rapidjson::StringBuffer> w(sb);
  target_doc.Accept(w);
  target = sb.GetString();
}

void JsonUtil::addArrayToJson(std::string &target,
                              const std::vector<std::string> &json_array,
                              const std::string &field_name) {
  std::string stringified_json_array = "[";

  if (json_array.size() > 0) {
    stringified_json_array += json_array[0];
    for (auto it = json_array.begin() + 1; it != json_array.end(); it++) {
      stringified_json_array += ",";
      stringified_json_array += *it;
    }
  }
  stringified_json_array += "]";

  mergeJsons(target, stringified_json_array, field_name);
}
} // namespace zipkin
