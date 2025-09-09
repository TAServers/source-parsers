#pragma once

#include <cstdint>

namespace VpkParser::Structs {
#pragma pack(push, 1)
  struct HeaderV1 {
    uint32_t signature = 0x55aa1234;
    uint32_t version = 1;

    /**
     * Size of the directory tree immediately following this header.
     */
    uint32_t directoryTreeSize = 0;
  };

  struct HeaderV2 {
    uint32_t signature = 0x55aa1234;
    uint32_t version = 2;

    /**
     * Size of the directory tree immediately following this header.
     */
    uint32_t directoryTreeSize = 0;

    /**
     * How many bytes of file content are stored in this VPK file.
     * @remark Always <c>0</c> in CSGO.
     */
    uint32_t fileDataSectionSize = 0;

    /**
     * The size, in bytes, of the section containing MD5 checksums for external archive content.
     */
    uint32_t archiveMd5SectionSize = 0;

    /**
     * The size, in bytes, of the section containing MD5 checksums for content in this file.
     * @remark Should always be <c>48</c>.
     */
    uint32_t otherMd5SectionSize = 0;

    /**
     * The size, in bytes, of the section containing the public key and signature.
     * @remark This is either <c>0</c> (CSGO & The Ship) or <c>296</c> (HL2, HL2:DM, HL2:EP1, HL2:EP2, HL2:LC, TF2, DOD:S & CS:S).
     */
    uint32_t signatureSectionSize = 0;
  };
#pragma pack(pop)
}
