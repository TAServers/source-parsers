#pragma once

#include <format>
#include <span>
#include <string>
#include <variant>
#include <vector>
#include <optional>
#include <source-parsers-shared/internal/check-bounds.hpp>
#include <source-parsers-shared/internal/offset-data-view.hpp>
#include "errors.hpp"
#include "phys-model.hpp"
#include "displacements/triangulated-displacement.hpp"
#include "enums/lump.hpp"
#include "helpers/lzma-callback.hpp"
#include "helpers/zip.hpp"
#include "structs/common.hpp"
#include "structs/detail-props.hpp"
#include "structs/displacements.hpp"
#include "structs/geometry.hpp"
#include "structs/headers.hpp"
#include "structs/models.hpp"
#include "structs/static-props.hpp"
#include "structs/textures.hpp"
#include "structs/lzma-header.hpp"

namespace BspParser {
  /**
   * Lightweight abstraction over a BSP file, providing direct access to many of its lumps without any additional allocations.
   *
   * @note Does not take ownership of the passed data. It is your responsibility to ensure the lifetime of the BSP does not exceed that of the underlying data.
   */
  struct Bsp {
    explicit Bsp(
      std::span<const std::byte> data,
      std::optional<LZMADecompressCallback> lzmaDecompressCallback = std::nullopt
    );

    std::span<const std::byte> data;

    const Structs::Header* header = nullptr;

    std::span<const Structs::GameLump> gameLumps;

    std::span<const Structs::Vector> vertices;
    std::span<const Structs::Plane> planes;
    std::span<const Structs::Edge> edges;
    std::span<const int32_t> surfaceEdges;
    std::span<const Structs::Face> faces;

    std::span<const Structs::TexInfo> textureInfos;
    std::span<const Structs::TexData> textureDatas;
    std::span<const int32_t> textureStringTable;
    std::span<const char> textureStringData;

    std::span<const Structs::Model> models;

    std::span<const Structs::DispInfo> displacementInfos;
    std::span<const Structs::DispVert> displacementVertices;

    /**
     * Triangulated and internally smoothed displacement infos for rendering.
     * @note Use smoothNeighbouringDisplacements to smooth the normals and tangents between connected displacements, which mutates this collection.
     */
    std::vector<TriangulatedDisplacement> displacements;

    std::vector<PhysModel> physicsModels;

    std::vector<Zip::ZipFileEntry> compressedPakfile;

    std::vector<std::vector<std::byte>> decompressedLumps;
    std::optional<LZMADecompressCallback> lzmaDecompressCallback = std::nullopt;

    // std::span<const Structs::DetailObjectDict> detailObjectDictionary;
    // std::span<const Structs::DetailObject> detailObjects;

    std::optional<std::span<const Structs::StaticPropDict>> staticPropDictionary = std::nullopt;
    std::optional<std::span<const Structs::StaticPropLeaf>> staticPropLeaves = std::nullopt;

    std::optional<std::variant<
      std::span<const Structs::StaticPropV4>,
      std::span<const Structs::StaticPropV5>,
      std::span<const Structs::StaticPropV6>,
      std::span<const Structs::StaticPropV7Multiplayer2013>>>
    staticProps = std::nullopt;

    /**
     * Smooths normals and tangents between neighbouring displacements for rendering.
     * @warning This must only be called once.
     */
    void smoothNeighbouringDisplacements();

  private:
    template<typename LumpType>
    std::span<const LumpType> decompressLump(Enums::Lump lump, const std::span<const LumpType> lumpSpan) {
      if (!lzmaDecompressCallback) {
        throw Errors::MissingDecompressCallback(
          lump,
          "Encountered an LZMA-compressed lump but no LZMA decompression callback was provided"
        );
      }

      const auto lumpData = std::as_bytes(lumpSpan);
      const auto offsetDataView = SourceParsers::Internal::OffsetDataView(lumpData);
      const auto header = offsetDataView.parseStruct<Structs::LzmaHeader>(
        0,
        "Failed to parse LZMA header for compressed lump"
      );

      const auto callback = lzmaDecompressCallback.value();
      const auto metadata = LZMAMetadata{
        .uncompressedSize = header.uncompressedSize,
        .properties = {
          header.properties[0],
          header.properties[1],
          header.properties[2],
          header.properties[3],
          header.properties[4]
        }
      };

      decompressedLumps.push_back(callback(lumpData.subspan(sizeof(Structs::LzmaHeader)), metadata));

      const auto& decompressedData = decompressedLumps.back();
      return std::span<const LumpType>(
        reinterpret_cast<const LumpType*>(decompressedData.data()),
        header.uncompressedSize / sizeof(LumpType)
      );
    }

    bool isLumpCompressed(Enums::Lump lump) const {
      const auto& lumpHeader = header->lumps.at(static_cast<size_t>(lump));
      return lumpHeader.fourCC != 0;
    }

    template<typename LumpType>
    std::span<const LumpType> parseLump(Enums::Lump lump, size_t maxItems = std::numeric_limits<size_t>::max()) {
      const auto& lumpHeader = header->lumps.at(static_cast<size_t>(lump));

      assertLumpHeaderValid(lump, lumpHeader);

      const auto lumpLength = isLumpCompressed(lump)
        ? lumpHeader.fourCC
        : lumpHeader.length;

      if (lumpLength % sizeof(LumpType) != 0) {
        throw Errors::InvalidBody(
          lump,
          std::format(
            "Lump header has length ({}) which is not a multiple of the size of its item type ({})",
            lumpLength,
            sizeof(LumpType)
          )
        );
      }

      const auto numItems = lumpLength / sizeof(LumpType);
      if (numItems > maxItems) {
        throw Errors::InvalidBody(
          lump,
          std::format("Number of lump items ({}) exceeds source engine maximum ({})", numItems, maxItems)
        );
      }

      const auto lumpSpan = std::span<const LumpType>(
        reinterpret_cast<const LumpType*>(&data[lumpHeader.offset]),
        numItems
      );

      return isLumpCompressed(lump)
        ? decompressLump(lump, lumpSpan)
        : lumpSpan;
    }

    [[nodiscard]] std::span<const Structs::GameLump> parseGameLumpHeaders() const;

    [[nodiscard]] std::vector<PhysModel> parsePhysCollideLump();

    template<class StaticProp>
    [[nodiscard]] std::span<const StaticProp> parseStaticPropLump(const Structs::GameLump& lumpHeader) {
      if (lumpHeader.offset < 0) {
        throw Errors::InvalidBody(
          Enums::Lump::GameLump,
          std::format("Static prop game lump header has a negative offset ({})", lumpHeader.offset)
        );
      }

      if (lumpHeader.length < 0) {
        throw Errors::InvalidBody(
          Enums::Lump::GameLump,
          std::format("Static prop game lump header has a negative length ({})", lumpHeader.length)
        );
      }

      if (lumpHeader.offset + lumpHeader.length > data.size_bytes()) {
        throw Errors::OutOfBoundsAccess(
          Enums::Lump::GameLump,
          std::format(
            "Static prop game lump header has offset + length ({}) overrunning the file ({})",
            lumpHeader.offset + lumpHeader.length,
            data.size_bytes()
          )
        );
      }

      const auto isGameLumpCompressed = lumpHeader.flags & Structs::GameLump::COMPRESSED_FLAG != 0;
      const auto gameLumpSpan = data.subspan(lumpHeader.offset, lumpHeader.length);

      const auto dictionaryData = SourceParsers::Internal::OffsetDataView(
        isGameLumpCompressed
        ? decompressLump(
          Enums::Lump::GameLump,
          gameLumpSpan
        )
        : gameLumpSpan
      );

      const auto numDictionaryEntries = dictionaryData.parseStruct<int32_t>(
        0,
        "Static prop game lump length is shorter than a single int32 for the dictionary count"
      );
      staticPropDictionary = dictionaryData.parseStructArray<Structs::StaticPropDict>(
        sizeof(int32_t),
        numDictionaryEntries,
        "Static prop game lump dictionary entries overflowed the lump"
      );

      const auto leafData =
        dictionaryData.withRelativeOffset(sizeof(int32_t) + numDictionaryEntries * sizeof(Structs::StaticPropDict));
      const auto numLeaves = leafData.parseStruct<int32_t>(
        0,
        "Static prop game lump length is shorter than its dictionary entries plus a single int32 for the leaf count"
      );
      staticPropLeaves = leafData.parseStructArray<Structs::StaticPropLeaf>(
        sizeof(int32_t),
        numLeaves,
        "Static prop game lump leaves overflowed the lump"
      );

      const auto propData = leafData.withRelativeOffset(sizeof(int32_t) + numLeaves * sizeof(Structs::StaticPropLeaf));
      const auto numProps = propData.parseStruct<int32_t>(
        0,
        "Static prop game lump length is shorter than its dictionary, leaves, and a single int32 for the prop count"
      );
      const auto props = propData.parseStructArray<StaticProp>(
        sizeof(int32_t),
        numProps,
        "Static prop game lump props overflowed the lump"
      );

      return props;
    }

    [[nodiscard]] std::vector<Zip::ZipFileEntry> parsePakfileLump();

    void assertLumpHeaderValid(Enums::Lump lump, const Structs::Lump& lumpHeader) const;

    [[nodiscard]] TriangulatedDisplacement createTriangulatedDisplacement(
      const Structs::DispInfo& displacementInfo
    ) const;
  };
}
