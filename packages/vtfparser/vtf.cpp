#include "vtf.hpp"
#include <cstring>
#include <utility>
#include <source-parsers-shared/internal/check-bounds.hpp>
#include <source-parsers-shared/errors.hpp>

namespace VtfParser {
  using namespace SourceParsers::Errors;
  using namespace SourceParsers::Internal;

  namespace {
    constexpr std::array<char, 4> FILE_ID = { 'V', 'T', 'F', 0 };

    constexpr uint32_t SUPPORTED_MAJOR_VERSION = 7;
    constexpr uint32_t MIN_SUPPORTED_MINOR_VERSION = 0;
    constexpr uint32_t MAX_SUPPORTED_MINOR_VERSION = 5;

    /**
     * First minor version with resource infos.
     */
    constexpr uint32_t MIN_RESOURCE_INFO_MINOR_VERSION = 3;

    constexpr std::array<uint8_t, 3> LOW_RES_RESOURCE_TAG = { 0x01, 0, 0 };
    constexpr std::array<uint8_t, 3> HIGH_RES_RESOURCE_TAG = { 0x30, 0, 0 };

    struct ImageSizeInfo {
      ImageFormat format;
      size_t width;
      size_t height;
      size_t depth;
      size_t faces;
      size_t frames;
      uint8_t mipLevels;
    };

    /**
     * Returns the number of bytes used by each pixel of the given format.
     * @remark Only works with uncompressed formats.
     * @param format
     * @return Size of each pixel in bytes.
     */
    size_t getPixelSizeBytes(const ImageFormat format) {
      switch (format) {
        case ImageFormat::RGBA16161616F:
        case ImageFormat::RGBA16161616:
          return 8;
        case ImageFormat::RGBA8888:
        case ImageFormat::ABGR8888:
        case ImageFormat::ARGB8888:
        case ImageFormat::BGRA8888:
        case ImageFormat::BGRX8888:
        case ImageFormat::UVWQ8888:
        case ImageFormat::UVLX8888:
          return 4;
        case ImageFormat::RGB888:
        case ImageFormat::BGR888:
        case ImageFormat::RGB888_BLUESCREEN:
        case ImageFormat::BGR888_BLUESCREEN:
          return 3;
        case ImageFormat::RGB565:
        case ImageFormat::IA88:
        case ImageFormat::BGR565:
        case ImageFormat::BGRX5551:
        case ImageFormat::BGRA4444:
        case ImageFormat::BGRA5551:
        case ImageFormat::UV88:
          return 2;
        case ImageFormat::I8:
        case ImageFormat::P8:
        case ImageFormat::A8:
          return 1;
        default:
          throw InvalidHeader("Unrecognised image format");
      }
    }

    size_t getSliceSizeBytes(const ImageSizeInfo& sizeInfo) {
      switch (sizeInfo.format) {
        case ImageFormat::DXT1:
        case ImageFormat::DXT1_ONEBITALPHA: {
          const auto width = std::max<size_t>(sizeInfo.width, 4);
          const auto height = std::max<size_t>(sizeInfo.height, 4);

          return ((width + 3u) >> 2u) * ((height + 3u) >> 2u) * 8u;
        }
        case ImageFormat::DXT3:
        case ImageFormat::DXT5: {
          const auto width = std::max<size_t>(sizeInfo.width, 4);
          const auto height = std::max<size_t>(sizeInfo.height, 4);

          return ((width + 3u) >> 2u) * ((height + 3u) >> 2u) * 16u;
        }
        default:
          return sizeInfo.width * sizeInfo.height * getPixelSizeBytes(sizeInfo.format);
      }
    }

    size_t getFaceSizeBytes(const ImageSizeInfo& sizeInfo) {
      return getSliceSizeBytes(sizeInfo) * sizeInfo.depth;
    }

    size_t getFrameSizeBytes(const ImageSizeInfo& sizeInfo) {
      return getFaceSizeBytes(sizeInfo) * sizeInfo.faces;
    }

    size_t getMipSizeBytes(const ImageSizeInfo& sizeInfo) {
      return getFrameSizeBytes(sizeInfo) * sizeInfo.frames;
    }

    size_t getImageSizeBytes(const ImageSizeInfo& sizeInfo) {
      size_t size = 0;
      for (uint8_t mipLevel = 0; mipLevel < sizeInfo.mipLevels; mipLevel++) {
        auto mipSizeInfo = sizeInfo;
        mipSizeInfo.width = std::max<size_t>(sizeInfo.width >> mipLevel, 1ul);
        mipSizeInfo.height = std::max<size_t>(sizeInfo.height >> mipLevel, 1ul);
        mipSizeInfo.depth = std::max<size_t>(sizeInfo.depth >> mipLevel, 1ul);
        size += getMipSizeBytes(mipSizeInfo);
      }

      return size;
    }
  }

  Vtf::Vtf(const std::span<const std::byte> data) {
    checkBounds(0, sizeof(Header), data.size(), "Failed to parse VTF header");
    header = *reinterpret_cast<const Header*>(data.data());

    if (memcmp(header.signature.data(), FILE_ID.data(), 4) != 0) {
      throw InvalidHeader("VTF header has an invalid file ID");
    }

    if (header.version[0] != SUPPORTED_MAJOR_VERSION || header.version[1] < MIN_SUPPORTED_MINOR_VERSION || header.
      version[1] > MAX_SUPPORTED_MINOR_VERSION) {
      throw UnsupportedVersion("VTF version is not supported");
    }

    // Fix-up for old versions of the format, which put garbage data here
    if (header.version[1] < 2) {
      header.depth = 1;
    }
    if (header.version[1] < MIN_RESOURCE_INFO_MINOR_VERSION) {
      header.numResources = 0;
    }

    if (header.highResImageFormat == ImageFormat::NONE) {
      throw InvalidHeader("VTF high res image format is NONE");
    }

    if (header.numResources > Header::MAX_RESOURCES) {
      throw InvalidHeader("VTF resource count is higher than maximum allowed");
    }

    const auto lowResImageDataSize = getImageSizeBytes(
      ImageSizeInfo{
        .format = header.lowResImageFormat,
        .width = header.lowResImageWidth,
        .height = header.lowResImageHeight,
        .depth = 1,
        .faces = 1,
        .frames = 1,
        .mipLevels = 1,
      }
    );
    const auto highResImageDataSize = getImageSizeBytes(
      {
        .format = header.highResImageFormat,
        .width = header.width,
        .height = header.height,
        .depth = header.depth,
        .faces = getFaces(),
        .frames = header.frames,
        .mipLevels = header.mipmapCount,
      }
    );

    if (header.version[1] >= MIN_RESOURCE_INFO_MINOR_VERSION) {
      for (const auto& resourceInfo : std::span(header.resourceInfos).subspan(0, header.numResources)) {
        if (resourceInfo.tag == LOW_RES_RESOURCE_TAG) {
          lowResImageData = data.subspan(resourceInfo.data, lowResImageDataSize);
        } else if (resourceInfo.tag == HIGH_RES_RESOURCE_TAG) {
          highResImageData = data.subspan(resourceInfo.data, highResImageDataSize);
        }
      }
    } else {
      lowResImageData = data.subspan(header.headerSize, lowResImageDataSize);
      highResImageData = data.subspan(header.headerSize + lowResImageDataSize, highResImageDataSize);
    }
  }

  ImageFormat Vtf::getHighResImageFormat() const {
    return header.highResImageFormat;
  }

  Vtf::HighResImageExtent Vtf::getHighResImageExtent(const uint8_t mipLevel) const {
    return {
      .width = std::max<uint16_t>(header.width >> mipLevel, 1),
      .height = std::max<uint16_t>(header.height >> mipLevel, 1),
      .depth = std::max<uint16_t>(header.depth >> mipLevel, 1),
    };
  }

  uint8_t Vtf::getFaces() const {
    if ((header.flags & TextureFlags::ENVMAP) == TextureFlags::NONE) {
      return 1;
    }

    return header.firstFrame == 0xffff && header.version[1] < 5 ? 7 : 6;
  }

  uint8_t Vtf::getMipLevels() const {
    return header.mipmapCount;
  }

  uint16_t Vtf::getFrames() const {
    return header.frames;
  }

  uint16_t Vtf::getFirstFrame() const {
    return header.firstFrame;
  }

  TextureFlags Vtf::getFlags() const {
    return header.flags;
  }

  std::span<const std::byte> Vtf::getHighResImageData() const {
    return highResImageData;
  }

  size_t Vtf::getImageSliceOffset(
    const uint8_t mipLevel,
    const uint16_t frame,
    const uint8_t face,
    const uint16_t depth
  ) const {
    const auto targetExtent = getHighResImageExtent(mipLevel);
    const ImageSizeInfo targetMipSize = {
      .format = getHighResImageFormat(),
      .width = targetExtent.width,
      .height = targetExtent.height,
      .depth = targetExtent.depth,
      .faces = getFaces(),
      .frames = getFrames(),
      .mipLevels = getMipLevels(),
    };

    size_t offset = 0;
    for (uint8_t i = mipLevel + 1; i < targetMipSize.mipLevels; i++) {
      const auto mipExtent = getHighResImageExtent(i);
      auto mipSize = targetMipSize;
      mipSize.width = mipExtent.width;
      mipSize.height = mipExtent.height;
      mipSize.depth = mipExtent.depth;

      offset += getMipSizeBytes(mipSize);
    }

    offset += getFrameSizeBytes(targetMipSize) * frame;
    offset += getFaceSizeBytes(targetMipSize) * face;
    offset += getSliceSizeBytes(targetMipSize) * depth;

    return offset;
  }

  ImageFormat Vtf::getLowResImageFormat() const {
    return header.lowResImageFormat;
  }

  Vtf::LowResImageExtent Vtf::getLowResImageExtent() const {
    return {
      .width = header.lowResImageWidth,
      .height = header.lowResImageHeight,
    };
  }

  std::span<const std::byte> Vtf::getLowResImageData() const {
    return lowResImageData;
  }
}
