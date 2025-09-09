#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <source-parsers-shared/internal/case-insensitive-map.hpp>

namespace VdfParser {
  struct KeyValue {
    std::variant<std::string, SourceParsers::Internal::CaseInsensitiveMap<KeyValue>> value;

    [[nodiscard]] std::optional<SourceParsers::Internal::CaseInsensitiveMap<KeyValue>> getChildren() const;

    [[nodiscard]] std::optional<KeyValue> getChild(const std::string& key) const;

    [[nodiscard]] bool hasChild(const std::string& key) const;

    [[nodiscard]] std::optional<std::string> getValue() const;

    [[nodiscard]] std::optional<std::string> getNestedValue(const std::vector<std::string>& path) const;
  };
}
