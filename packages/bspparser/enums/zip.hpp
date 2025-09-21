#pragma once
#include <cstdint>

namespace BspParser::Enums {
  /**
   * https://sourcegraph.com/github.com/lua9520/source-engine-2018-hl2_src@3bf9df6b2785fa6d951086978a3e66f49427166a/-/blob/public/zip_utils.h?L26:3-26:24
   */
  enum class ZipCompressionMethod : uint16_t {
    Unknown = -1,
    None = 0,
    LZMA = 14
  };
}
