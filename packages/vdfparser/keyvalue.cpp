#include "keyvalue.hpp"

namespace VdfParser {
  using namespace SourceParsers::Internal;

  std::optional<CaseInsensitiveMap<KeyValue>> KeyValue::getChildren() const {
    if (!std::holds_alternative<CaseInsensitiveMap<KeyValue>>(value)) {
      return std::nullopt;
    }

    return std::get<CaseInsensitiveMap<KeyValue>>(value);
  }

  std::optional<KeyValue> KeyValue::getChild(const std::string& key) const {
    const auto& children = getChildren();
    if (!children.has_value()) {
      return std::nullopt;
    }

    return children->contains(key) ? std::make_optional(children->at(key)) : std::nullopt;
  }

  bool KeyValue::hasChild(const std::string& key) const {
    const auto& children = getChildren();
    if (!children.has_value()) {
      return false;
    }

    return children->contains(key);
  }

  std::optional<std::string> KeyValue::getValue() const {
    return std::holds_alternative<std::string>(value) ? std::make_optional(std::get<std::string>(value)) : std::nullopt;
  }

  std::optional<std::string> KeyValue::getNestedValue(const std::vector<std::string>& path) const {
    std::optional node = *this;

    for (const auto& key : path) {
      node = node->getChild(key);
      if (!node.has_value()) {
        return std::nullopt;
      }
    }

    return node->getValue();
  }
}
