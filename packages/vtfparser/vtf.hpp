#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include "file-format-objects/header.hpp"

namespace VtfParser {
  /**
   * Abstracts parsing and accessing the contents of the Valve Texture Format (VTF).
   */
  class Vtf {
  public:
    /**
     * Extents of the low resolution (thumbnail) image.
     */
    struct LowResImageExtent {
      /**
       * Width of the image in pixels.
       */
      uint8_t width;
      /**
       * Height of the image in pixels.
       */
      uint8_t height;
    };

    /**
     * Extents of the high resolution image.
     */
    struct HighResImageExtent {
      /**
       * Width of the image in pixels.
       */
      uint16_t width;
      /**
       * Height of the image in pixels.
       */
      uint16_t height;
      /**
       * Depth of the image in pixels. Will be 1 unless the texture is volumetric.
       */
      uint16_t depth;
    };

    /**
     * Loads the VTF given by the binary data into an easily accessible structure.
     * Does not take ownership of the data.
     * @param data
     */
    explicit Vtf(std::span<const std::byte> data);

    /**
     * Gets the format of the high resolution image data.
     * @return ImageFormat enum.
     */
    [[nodiscard]] ImageFormat getHighResImageFormat() const;

    /**
     * Gets the extents (size on each axis) of the high resolution image data at the given mip level.
     * @param mipLevel Mipmap to get the extents of.
     * @return Width, height and depth of the image's mip level.
     */
    [[nodiscard]] HighResImageExtent getHighResImageExtent(uint8_t mipLevel = 0) const;

    /**
     * Gets the number of cubemap faces in the image.
     * @return 6 or 7 for cubemaps (depending on version) and 1 for anything else
     */
    [[nodiscard]] uint8_t getFaces() const;

    /**
     * Gets the number of levels in the image's mipmap chain.
     * @return Number of mipmap levels.
     */
    [[nodiscard]] uint8_t getMipLevels() const;

    /**
     * Gets the number of frames of animation in the image.
     * @return Number of frames.
     */
    [[nodiscard]] uint16_t getFrames() const;
    /**
     * Gets the frame of animation to start on.
     * @return First frame of the animation.
     */
    [[nodiscard]] uint16_t getFirstFrame() const;

    /**
     * Gets the flags set on the texture.
     * @return TextureFlags packed together as a bitflag.
     */
    [[nodiscard]] TextureFlags getFlags() const;

    /**
     * Gets the high resolution image data.
     * @return View over the high-res data.
     */
    [[nodiscard]] std::span<const std::byte> getHighResImageData() const;

    /**
     * Gets the offset to an image slice at the given mipmap level, animation frame, cubemap face and depth in the high res image data.
     * @remark Each image slice is a regular 2D image and may be in any of the supported formats,
     * @remark including compressed formats using DXT1, DXT3 or DXT5. All slices have the same format.
     * @param mipLevel Level of the mipmap chain.
     * @param frame Frame of animation.
     * @param face Face of a cubemap.
     * @param depth Depth or Z value of a volumetric texture.
     * @return Offset (in bytes) into the data returned by getHighResImageData().
     */
    [[nodiscard]] size_t getImageSliceOffset(
      uint8_t mipLevel = 0,
      uint16_t frame = 0,
      uint8_t face = 0,
      uint16_t depth = 0
    ) const;

    /**
     * Gets the format of the low resolution image data.
     * @remark This is almost always DXT1.
     * @return ImageFormat enum.
     */
    [[nodiscard]] ImageFormat getLowResImageFormat() const;

    /**
     * Gets the extents (size on each axis) of the low resolution image data.
     * @remark Unlike the high resolution image, the low res image has no mipmaps.
     * @return Width and height of the low resolution image.
     */
    [[nodiscard]] LowResImageExtent getLowResImageExtent() const;

    /**
     * Gets the low resolution image data.
     * @return View over the low res data.
     */
    [[nodiscard]] std::span<const std::byte> getLowResImageData() const;

  private:
    Header header{};

    std::span<const std::byte> highResImageData;
    std::span<const std::byte> lowResImageData;
  };
}
