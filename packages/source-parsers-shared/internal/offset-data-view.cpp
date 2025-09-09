#include "offset-data-view.hpp"

namespace SourceParsers::Internal {
  OffsetDataView::OffsetDataView(const std::span<const std::byte> data) : data(data), offset(0) {}

  OffsetDataView::OffsetDataView(const OffsetDataView& from, const size_t newOffset)
    : data(from.data), offset(newOffset) {}

  OffsetDataView OffsetDataView::withAbsoluteOffset(const size_t newOffset) const {
    checkBounds(newOffset, 1, data.size_bytes(), "Invalid offset passed to withAbsoluteOffset");

    return OffsetDataView(*this, newOffset);
  }

  OffsetDataView OffsetDataView::withRelativeOffset(const int64_t newOffset) const {
    const auto newAbsoluteOffset = getAbsoluteOffset(newOffset, 1, "Invalid offset passed to withRelativeOffset");

    return OffsetDataView(*this, newAbsoluteOffset);
  }

  std::string_view OffsetDataView::parseString(const int64_t relativeOffset, const char* errorMessage) const {
    const auto absoluteOffset = getAbsoluteOffset(relativeOffset, 1, errorMessage);

    for (size_t length = 0; absoluteOffset + length < data.size_bytes(); length++) {
      if (data[absoluteOffset + length] == static_cast<std::byte>(0)) {
        return std::string_view(reinterpret_cast<const char*>(&data[absoluteOffset]), length);
      }
    }

    throw Errors::OutOfBoundsAccess(errorMessage);
  }

  size_t OffsetDataView::getAbsoluteOffset(
    const int64_t relativeOffset,
    const size_t readSizeBytes,
    const char* errorMessage
  ) const {
    const auto absoluteOffset = static_cast<int64_t>(offset) + relativeOffset;
    checkBounds(absoluteOffset, readSizeBytes, data.size_bytes(), errorMessage);

    return absoluteOffset;
  }
}
