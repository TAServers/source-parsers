#pragma once

#include <cstddef>
#include "shared/errors.hpp"

namespace SourceParsers::Internal {
  inline void checkBounds(const size_t offset, const size_t count, const size_t rangeSize, const char* errorMessage) {
    if (offset >= rangeSize || offset + count > rangeSize) {
      throw Errors::OutOfBoundsAccess(errorMessage);
    }
  }
}
