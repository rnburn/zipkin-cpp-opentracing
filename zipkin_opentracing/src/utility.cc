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
struct ValueVisitor {
  BinaryAnnotation &annotation;
  const Value &original_value;

  void operator()(bool value) const {
    annotation.setValue(std::to_string(value));
  }

  void operator()(double value) const {
    annotation.setValue(std::to_string(value));
  }

  void operator()(int64_t value) const {
    annotation.setValue(std::to_string(value));
  }

  void operator()(uint64_t value) const {
    // There's no uint64_t value type so cast to an int64_t.
    annotation.setValue(std::to_string(value));
  }

  void operator()(const std::string &s) const { annotation.setValue(s); }

  void operator()(std::nullptr_t) const { annotation.setValue("0"); }

  void operator()(const char *s) const { annotation.setValue(std::string{s}); }

  void operator()(const Values & /*unused*/) const {
    annotation.setValue(toJson(original_value));
  }

  void operator()(const Dictionary & /*unused*/) const {
    annotation.setValue(toJson(original_value));
  }
};
} // anonymous namespace

BinaryAnnotation toBinaryAnnotation(string_view key, const Value &value) {
  BinaryAnnotation annotation;
  annotation.setKey(key);
  ValueVisitor value_visitor{annotation, value};
  apply_visitor(value_visitor, value);
  return annotation;
}
} // namespace zipkin
