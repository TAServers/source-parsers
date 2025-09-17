#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include "enums.hpp"

#ifdef _MSC_VER
#define align(x) alignas(x)
#else
#define align(x) __attribute__((aligned(x)))
#endif

namespace VtfParser {
#pragma pack(push, 1)

  /**
   * Base header struct.
   */
  struct HeaderBase {
    /**
     * File signature ("VTF\0"). (or as little-endian integer, 0x00465456)
     */
    std::array<uint8_t, 4> signature;
    /**
     * File format version. Index zero is MAJOR and one is MINOR.
     */
    std::array<uint32_t, 2> version;
    /**
     * Size of the header struct  (16 byte aligned; currently 80 bytes) + size of the resources dictionary (7.3+).
     */
    uint32_t headerSize;
  };

  /**
   * Aligned full header (only for alignment reasons, do not use outside of Header declaration).
   */
  struct align(16) HeaderFullAligned : public HeaderBase {
    /**
     * Width of the largest mipmap in pixels. Must be a power of 2.
     */
    uint16_t width;
    /**
     * Height of the largest mipmap in pixels. Must be a power of 2.
     */
    uint16_t height;
    /**
     * VTF flags.
     */
    TextureFlags flags;
    /**
     * Number of frames, if animated (1 for no animation).
     */
    uint16_t frames;
    /**
     * First frame in animation (0 based). Can be -1 in environment maps older than 7.5, meaning there are 7 faces, not 6.
     */
    uint16_t firstFrame;
    /**
     * Reflectivity padding (16 byte alignment).
     */
    std::array<uint8_t, 4> padding0;
    /**
     * Reflectivity vector.
     */
    std::array<float, 3> reflectivity;
    /**
     * Reflectivity padding (8 byte packing).
     */
    std::array<uint8_t, 4> padding1;
    /**
     * Bumpmap scale.
     */
    float bumpmapScale;
    /**
     * Format of the high resolution image.
     */
    ImageFormat highResImageFormat;
    /**
     * Number of MIP levels.
     */
    uint8_t mipmapCount;
    /**
     * Format of the low resolution image (always DXT1).
     */
    ImageFormat lowResImageFormat;
    /**
     * Low resolution image width.
     */
    uint8_t lowResImageWidth;
    /**
     * Low resolution image height.
     */
    uint8_t lowResImageHeight;
    /**
     * Depth of the largest mipmap in pixels. Must be a power of 2. Is 1 for a 2D texture.
     */
    uint16_t depth;
    /**
     * Depth padding (4 byte alignment).
     */
    std::array<uint8_t, 3> padding2;
    /**
     * Number of resources the VTF has. The max appears to be 32.
     */
    uint32_t numResources;
  };

  /**
   * Metadata about a particular resource.
   */
  struct ResourceEntryInfo {
    /**
     * A three-byte "tag" that identifies what this resource is.
     */
    std::array<uint8_t, 3> tag;
    /**
     * Resource entry flags.
     * The only known flag is 0x2, which indicates that no data chunk corresponds to this resource.
     */
    uint8_t flags;
    /**
     * The resource's data or the offset to its data in the file.
     */
    uint32_t data;
  };

  /**
   * VTF file header.
   */
  struct Header : public HeaderFullAligned {
    static constexpr size_t MAX_RESOURCES = 32;

    std::array<uint8_t, 8> padding3;
    std::array<ResourceEntryInfo, MAX_RESOURCES> resourceInfos;
  };

#pragma pack(pop)
}

#undef align