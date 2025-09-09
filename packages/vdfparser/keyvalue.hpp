#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <source-parsers-shared/internal/case-insensitive-map.hpp>

namespace VdfParser {
  /**
   * Abstraction over an entry in the VDF key-value structure.
   * May represent either a primitive string value, or a complex object.
   *
   * In most cases you'll want to use getNestedValue which lets you access a string value at a given path.
   */
  struct KeyValue {
    /**
     * The raw primitive or object value.
     */
    std::variant<std::string, SourceParsers::Internal::CaseInsensitiveMap<KeyValue>> value;

    /**
     * Returns all children of this object, or std::nullopt if this isn't an object value.
     * @return Children of this object.
     */
    [[nodiscard]] std::optional<SourceParsers::Internal::CaseInsensitiveMap<KeyValue>> getChildren() const;

    /**
     * Returns another KeyValue instance wrapping the value at the given key,
     * or std::nullopt if the key doesn't exist or this isn't an object value.
     * @param key Key to read.
     * @return Value of the key.
     */
    [[nodiscard]] std::optional<KeyValue> getChild(const std::string& key) const;

    /**
     * Returns whether a value exists at the give key.
     * @param key Key to check.
     * @return True if this is an object and the key exists, false otherwise.
     */
    [[nodiscard]] bool hasChild(const std::string& key) const;

    /**
     * Returns the value contained within this KeyValue instance, or std::nullopt if this is an object.
     * @return String value wrapped by this KeyValue instance.
     */
    [[nodiscard]] std::optional<std::string> getValue() const;

    /**
     * Returns the value at an arbitrarily deep path, or std::nullopt if any key in the path does not exist.
     * @param path List of keys to access recursively.
     * @return Value of the final key in the path.
     */
    [[nodiscard]] std::optional<std::string> getNestedValue(const std::vector<std::string>& path) const;
  };
}
