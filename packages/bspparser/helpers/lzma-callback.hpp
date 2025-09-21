#pragma once

#include <cstdint>
#include <vector>
#include <functional>
#include <span>
#include <array>

namespace BspParser {
  struct LzmaMetadata {
    uint32_t uncompressedSize;
    std::array<uint8_t, 5> properties;
  };

  using LzmaDecompressCallback = std::function<std::vector<std::byte>(
    std::span<const std::byte> compressedData,
    LzmaMetadata metadata
  )>;
}
