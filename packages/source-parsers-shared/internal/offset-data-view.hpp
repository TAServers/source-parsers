#pragma once

#include <memory>
#include <span>
#include <vector>

#include "check-bounds.hpp"

namespace SourceParsers::Internal {
  class OffsetDataView {
  public:
    template<typename T>
    using ValueOffsetPair = std::pair<T, size_t>;

    explicit OffsetDataView(std::span<const std::byte> data);

    explicit OffsetDataView(const OffsetDataView& from, size_t newOffset);

    ~OffsetDataView() = default;
    OffsetDataView(const OffsetDataView&) = delete;
    OffsetDataView& operator=(const OffsetDataView&) = delete;
    OffsetDataView(const OffsetDataView&&) = delete;
    OffsetDataView& operator=(const OffsetDataView&&) = delete;

    [[nodiscard]] OffsetDataView withAbsoluteOffset(size_t newOffset) const;

    [[nodiscard]] OffsetDataView withRelativeOffset(int64_t newOffset) const;

    template<typename T>
    [[nodiscard]] const T& parseStruct(const int64_t relativeOffset, const char* errorMessage) const {
      const auto absoluteOffset = getAbsoluteOffset(relativeOffset, sizeof(T), errorMessage);

      return *reinterpret_cast<const T*>(&data[absoluteOffset]);
    }

    template<typename T>
    [[nodiscard]] ValueOffsetPair<T> parseStructWithOffset(
      const int64_t relativeOffset,
      const char* errorMessage
    ) const {
      const auto absoluteOffset = getAbsoluteOffset(relativeOffset, sizeof(T), errorMessage);

      return std::make_pair(*reinterpret_cast<const T*>(&data[absoluteOffset]), absoluteOffset);
    }

    template<typename T>
    [[nodiscard]] std::span<const T> parseStructArray(
      const int64_t relativeOffset,
      const size_t count,
      const char* errorMessage
    ) const {
      if (count == 0) {
        return std::span<const T>(static_cast<const T*>(nullptr), 0);
      }

      const auto absoluteOffset = getAbsoluteOffset(relativeOffset, sizeof(T) * count, errorMessage);

      const T* first = reinterpret_cast<const T*>(&data[absoluteOffset]);
      return std::span<const T>(first, count);
    }

    template<typename T>
    [[nodiscard]] std::vector<ValueOffsetPair<T>> parseStructArrayWithOffsets(
      const int64_t relativeOffset,
      const size_t count,
      const char* errorMessage
    ) const {
      if (count == 0) {
        return {};
      }

      const auto absoluteOffset = getAbsoluteOffset(relativeOffset, sizeof(T) * count, errorMessage);

      std::vector<ValueOffsetPair<T>> parsed;
      parsed.reserve(count);

      for (size_t i = 0; i < count; i++) {
        const auto elementOffset = absoluteOffset + sizeof(T) * i;
        parsed.emplace_back(*reinterpret_cast<const T*>(&data[elementOffset]), elementOffset);
      }

      return std::move(parsed);
    }

    [[nodiscard]] std::string_view parseString(int64_t relativeOffset, const char* errorMessage) const;

  private:
    const std::span<const std::byte> data;
    const size_t offset;

    [[nodiscard]] size_t getAbsoluteOffset(
      int64_t relativeOffset,
      size_t readSizeBytes,
      const char* errorMessage
    ) const;
  };
}
