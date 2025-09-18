#pragma once

#include <cstdint>

namespace BspParser::Structs {
  struct LzmaHeader {
    static constexpr uint32_t LZMA_ID = 'A' << 24 | 'M' << 16 | 'Z' << 8 | 'L';

    uint32_t id;
    uint32_t uncompressedSize;
    uint32_t compressedSize;
    uint8_t properties[5];
  };
}
