#pragma once

#include <cstdint>
#include <vector>
#include <functional>
#include <span>

namespace BspParser {
  struct LZMAMetadata {
    uint32_t uncompressedSize;
    uint8_t properties[5];
  };

  using LZMADecompressCallback = std::function<std::vector<std::byte>(std::span<const std::byte> compressedData, LZMAMetadata metadata)>;
}
