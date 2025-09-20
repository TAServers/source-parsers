#pragma once

#include <array>
#include <optional>

#include "lzma-callback.hpp"
#include "../structs/zip.hpp"
#include <span>
#include <string_view>
#include <vector>

namespace BspParser::Zip {
  struct ZipFileLzmaMetadata {
    uint8_t majorVersion;
    uint8_t minorVersion;
    uint32_t uncompressedSize;
    std::array<uint8_t, 5> properties;
    uint8_t compressionHeaderSize = sizeof(Structs::Zip::CompressionPayload);
  };

  struct ZipFileEntry {
    Structs::Zip::FileHeader header;
    std::string_view fileName;
    std::span<const std::byte> data;
    std::optional<ZipFileLzmaMetadata> lzmaMetadata;
  };

  std::vector<ZipFileEntry> readZipFileEntries(std::span<const std::byte> zipData);
}
