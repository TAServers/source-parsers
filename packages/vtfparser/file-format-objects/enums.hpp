#pragma once

#include <cstdint>

namespace VtfParser {
  /**
   * Image formats.
   * @remark Many of these have the same size and components in memory, but should be interpreted differently.
   */
  enum class ImageFormat : int32_t // NOLINT(*-enum-size)
  {
    NONE = -1,
    RGBA8888 = 0,
    ABGR8888,
    RGB888,
    BGR888,
    RGB565,
    I8,
    IA88,
    P8,
    A8,
    RGB888_BLUESCREEN,
    BGR888_BLUESCREEN,
    ARGB8888,
    BGRA8888,
    DXT1,
    DXT3,
    DXT5,
    BGRX8888,
    BGR565,
    BGRX5551,
    BGRA4444,
    DXT1_ONEBITALPHA,
    BGRA5551,
    UV88,
    UVWQ8888,
    RGBA16161616F,
    RGBA16161616,
    UVLX8888
  };

  /**
   * Flags describing how the texture should be used (e.g. sampling mode).
   */
  enum class TextureFlags : uint32_t {
    /**
     * No flags are set
     */
    NONE = 0,
    /**
     * Texture should be rendered without interpolation (regardless of graphics options).
     */
    POINTSAMPLE = 0x00000001,
    /**
     * Texture should be rendered with trilinear interpolation (regardless of graphics options).
     */
    TRILINEAR = 0x00000002,
    /**
     * S texture coordinate should be clamped instead of wrapped.
     * @note ST is equivalent to UV.
     */
    CLAMPS = 0x00000004,
    /**
     * T texture coordinate should be clamped instead of wrapped.
     * @note ST is equivalent to UV.
     */
    CLAMPT = 0x00000008,
    /**
     * Texture should be rendered with anisotropic interpolation (regardless of graphics options).
     */
    ANISOTROPIC = 0x00000010,
    /**
     * Exact meaning unknown.
     */
    HINT_DXT5 = 0x00000020,
    /**
     * Usage unknown. Shares the same value as SRGB.
     */
    PWL_CORRECTED = 0x00000040,
    /**
     * Texture uses sRGB colour space.
     * @remark It's unclear whether this means the texture is stored as sRGB, or stored as linear and should be converted to sRGB.
     * @deprecated Deprecated since 7.5.
     */
    SRGB = 0x00000040,
    /**
     * Texture is a normal map.
     */
    NORMAL = 0x00000080,
    /**
     * Only the largest mip level should be used.
     * @remark The other mip levels are present in the file regardless of this option.
     */
    NOMIP = 0x00000100,
    /**
     * Not affected by texture resolution settings.
     */
    NOLOD = 0x00000200,
    /**
     * If set, use mipmaps below 32x32 pixels.
     * @remark Implies that by default only mip levels greater than 32x32 pixels in size should be used.
     */
    ALL_MIPS = 0x00000400,
    /**
     * Texture is a procedural texture.
     */
    PROCEDURAL = 0x00000800,

    /**
     * One bit alpha channel is used.
     */
    ONEBITALPHA = 0x00001000,
    /**
     * Eight bit alpha channel is used.
     * @note Actually used for all formats without a binary alpha channel.
     */
    EIGHTBITALPHA = 0x00002000,

    /**
     * Texture is an environment map.
     */
    ENVMAP = 0x00004000,
    /**
     * Texture is a rendertarget.
     */
    RENDERTARGET = 0x00008000,
    /**
     * Texture is a depth rendertarget.
     */
    DEPTHRENDERTARGET = 0x00010000,
    /**
     * Usage unknown.
     */
    NODEBUGOVERRIDE = 0x00020000,
    /**
     * Usage unknown.
     */
    SINGLECOPY = 0x00040000,
    /**
     * Texture has already had sRGB correction applied.
     * @remark Unclear if this means the texture data has been converted from sRGB to linear, or linear to sRGB - most likely the latter.
     */
    PRE_SRGB = 0x00080000,
    UNUSED_00100000 = 0x00100000,
    UNUSED_00200000 = 0x00200000,
    UNUSED_00400000 = 0x00400000,

    /**
     * Exclude from depth buffering.
     * @remark Presumably this means render above all other content on-screen.
     */
    NODEPTHBUFFER = 0x00800000,
    UNUSED_01000000 = 0x01000000,

    /**
     * U texture coordinate should be clamped instead of wrapped.
     * @attention This is the depth to sample in a volumetric (3D) texture, not the U in UV.
     */
    CLAMPU = 0x02000000,
    /**
     * Usable as a vertex texture.
     */
    VERTEXTEXTURE = 0x04000000,
    /**
     * Texture is an [SSBump](https://developer.valvesoftware.com/wiki/$ssbump).
     */
    SSBUMP = 0x08000000,
    UNUSED_10000000 = 0x10000000,

    /**
     * Clamp to border colour on all texture coordinates.
     */
    BORDER = 0x20000000,
    UNUSED_40000000 = 0x40000000,
    UNUSED_80000000 = 0x80000000
  };

  inline TextureFlags operator&(const TextureFlags& a, const TextureFlags& b) {
    return static_cast<TextureFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
  }

  inline TextureFlags operator|(const TextureFlags& a, const TextureFlags& b) {
    return static_cast<TextureFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
  }
}
