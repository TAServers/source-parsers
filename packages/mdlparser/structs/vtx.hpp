#pragma once

#include <array>
#include <cstdint>
#include "../enums.hpp"

namespace MdlParser::Structs::Vtx {
#pragma pack(push, 1)

  struct Vertex {
    std::array<uint8_t, 3> boneWeightIndex;
    uint8_t numBones;

    uint16_t origMeshVertId;

    std::array<int8_t, 3> boneId;
  };

  struct Strip {
    int32_t numIndices;
    int32_t indexOffset;

    int32_t numVerts;
    int32_t vertOffset;

    int16_t numBones;

    Enums::Vtx::StripFlags flags;

    int32_t numBoneStateChanges;
    int32_t boneStateChangeOffset;

    // this struct has 2 more ints if the MDL version is >= 49, however sdk 2013 uses version 48 (does gmod backport 49?)
  };

  struct StripGroup {
    int32_t numVerts;
    int32_t vertOffset;

    int32_t numIndices;
    int32_t indexOffset;

    int32_t numStrips;
    int32_t stripOffset;

    Enums::Vtx::StripGroupFlags flags;

    // this struct has 2 more ints if the MDL version is >= 49, however sdk 2013 uses version 48 (does gmod backport 49?)
  };

  struct Mesh {
    int32_t numStripGroups;
    int32_t stripGroupHeaderOffset;

    Enums::Vtx::MeshFlags flags;
  };

  struct ModelLoD {
    int32_t numMeshes;
    int32_t meshOffset;

    float switchPoint;
  };

  struct Model {
    int32_t numLoDs;
    int32_t lodOffset;
  };

  struct BodyPart {
    int32_t numModels;
    int32_t modelOffset;
  };

  struct MaterialReplacement {
    int16_t materialId;
    int32_t replacementMaterialNameOffset;
  };

  struct MaterialReplacementList {
    int32_t replacementCount;
    int32_t replacementOffset;
  };

  struct Header {
    static constexpr int32_t SUPPORTED_VERSION = 7;

    int32_t version;

    int32_t vertCacheSize;
    uint16_t maxBonesPerStrip;
    uint16_t maxBonesPerTri;
    int32_t maxBonesPerVert;

    int32_t checksum;

    int32_t numLoDs;

    int32_t materialReplacementListOffset;

    int32_t numBodyParts;
    int32_t bodyPartOffset;
  };

#pragma pack(pop)
}
