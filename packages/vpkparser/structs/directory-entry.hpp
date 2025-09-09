#pragma once

#include <cstdint>

namespace VpkParser::Structs {
#pragma pack(push, 1)
  struct DirectoryEntry {
    /**
     * CRC of the file this references.
     */
    uint32_t crc = 0;

    /**
     * Size of the preload data for this file in the index.
     */
    uint16_t preloadDataSize = 0;

    /**
     * Index of the VPK this file is contained in.
     * If <c>0x7FFF</c>, the data follows the directory tree.
     */
    uint16_t archiveIndex = 0;

    /**
     * Offset of the file's data from the start of the VPK referenced by <c>archiveIndex</c>,
     * or from the end of the directory tree if <c>archiveIndex</c> is <c>0x7FFF</c>.
     */
    uint32_t entryOffset = 0;

    /**
     * Size of the file data in bytes following from <c>entryOffset</c>.
     * @remarks If zero, then the entire file is stored in the preload data.
     */
    uint32_t entrySize = 0;

    uint16_t terminator = 0xffff;
  };
#pragma pack(pop)
}
