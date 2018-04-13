#include "utility.h"
#include <string>
#include <zipkin/rapidjson/stringbuffer.h>
#include <zipkin/rapidjson/writer.h>

using namespace opentracing;

namespace zipkin {
using JsonWriter = rapidjson::Writer<rapidjson::StringBuffer>;

static bool toJson(JsonWriter &writer, const Value &value);

namespace {
struct JsonValueVisitor {
  JsonWriter &writer;
  bool result;

  void operator()(bool value) { result = writer.Bool(value); }

  void operator()(double value) { result = writer.Double(value); }

  void operator()(int64_t value) { result = writer.Int64(value); }

  void operator()(uint64_t value) { result = writer.Uint64(value); }

  void operator()(const std::string &s) {
    result = writer.String(s.data(), static_cast<unsigned>(s.size()));
  }

  void operator()(std::nullptr_t) { result = writer.Null(); }

  void operator()(const char *s) { result = writer.String(s); }

  void operator()(const Values &values) {
    if (!(result = writer.StartArray())) {
      return;
    }
    for (const auto &value : values) {
      if (!(result = toJson(writer, value))) {
        return;
      }
    }
    result = writer.EndArray();
  }

  void operator()(const Dictionary &dictionary) {
    if (!(result = writer.StartObject())) {
      return;
    }
    for (const auto &key_value : dictionary) {
      if (!(result =
                writer.Key(key_value.first.data(),
                           static_cast<unsigned>(key_value.first.size())))) {
        return;
      }
      if (!(result = toJson(writer, key_value.second))) {
        return;
      }
    }
    result = writer.EndObject();
  }
};
} // anonymous namespace

static bool toJson(JsonWriter &writer, const Value &value) {
  JsonValueVisitor value_visitor{writer, true};
  apply_visitor(value_visitor, value);
  return value_visitor.result;
}

static std::string toJson(const Value &value) {
  rapidjson::StringBuffer buffer;
  JsonWriter writer(buffer);
  if (!toJson(writer, value)) {
    return {};
  }
  return buffer.GetString();
}

namespace {

struct ToStringValueVisitor {
  const Value &original_value;

  std::string operator()(bool value) const {
    return std::to_string(value);
  }

  std::string operator()(double value) const {
    return std::to_string(value);
  }

  std::string operator()(int64_t value) const {
    return std::to_string(value);
  }

  std::string operator()(uint64_t value) const {
    // There's no uint64_t value type so cast to an int64_t.
    return std::to_string(value);
  }

  std::string operator()(const std::string &s) const { return s; }

  std::string operator()(std::nullptr_t) const { return "0"; }

  std::string operator()(const char *s) const { return s; }

  std::string operator()(const Values & /*unused*/) const {
    return toJson(original_value);
  }

  std::string operator()(const Dictionary & /*unused*/) const {
    return toJson(original_value);
  }
};

} // anonymous namespace

BinaryAnnotation toBinaryAnnotation(string_view key, const Value &value) {
  ToStringValueVisitor value_visitor{value};
  const std::string str = apply_visitor(value_visitor, value);
  BinaryAnnotation annotation;
  annotation.setKey(key);
  annotation.setValue(str);
  return annotation;
}

Annotation toAnnotation(string_view key, const Value &value) {
  ToStringValueVisitor value_visitor{value};
  const std::string str = apply_visitor(value_visitor, value);
  Annotation annotation;
  annotation.setValue(std::string(key) + "=" + str);
  return annotation;
}

} // namespace zipkin
