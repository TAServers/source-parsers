#pragma once

#include <cstddef>
#include "source-parsers-shared/errors.hpp"

namespace SourceParsers::Internal {
  inline void checkBounds(const int64_t offset, const size_t count, const size_t rangeSize, const char* errorMessage) {
    if (offset < 0) {
      throw Errors::OutOfBoundsAccess(errorMessage);
    }

    const auto unsignedOffset = static_cast<size_t>(offset);

    if (unsignedOffset >= rangeSize || unsignedOffset + count > rangeSize) {
      throw Errors::OutOfBoundsAccess(errorMessage);
    }
  }
}
