#pragma once

#include "common.hpp"
#include "../limits.hpp"

namespace MdlParser::Structs::Vvd {
  using Limits::MAX_NUM_LODS;
  using Limits::MAX_NUM_BONES_PER_VERT;

#pragma pack(push, 1)

  struct Header {
    static constexpr int32_t SUPPORTED_VERSION = 4;

    int32_t id;
    int32_t version;
    int32_t checksum;

    int32_t numLoDs;
    std::array<int32_t, MAX_NUM_LODS> numLoDVertices;

    int32_t numFixups;
    int32_t fixupTableOffset;

    int32_t vertexDataOffset;
    int32_t tangentDataOffset;
  };

  struct Fixup {
    int32_t lod;
    int32_t sourceVertexId;
    int32_t numVertices;
  };

  struct BoneWeight {
    std::array<float, MAX_NUM_BONES_PER_VERT> weight;
    std::array<int8_t, MAX_NUM_BONES_PER_VERT> bone;
    uint8_t numBones;
  };

  struct Vertex {
    BoneWeight boneWeights;
    Vector pos;
    Vector normal;
    Vector2D texCoord;
  };

#pragma pack(pop)
}
