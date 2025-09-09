#pragma once

#include <array>
#include <cstdint>
#include "common.hpp"
#include "../enums.hpp"

namespace PhyParser::Enums {
  enum class ModelType : int16_t;
}
namespace PhyParser::Structs {
#pragma pack(push, 1)
  struct Header {
    int32_t size;
    int32_t id;
    int32_t solidCount;
    int64_t checksum;
  };

  struct SurfaceHeader {
    int32_t size;
    int32_t vphysicsId;
    int16_t version;
    Enums::ModelType modelType;
    int32_t surfaceSize;
  };

  struct CompactSurfaceHeader {
    Vector3 dragAxisAreas;
    int32_t axisMapSize;

    // Split into a separate "legacy surface header" by dev wiki, but always present for compact surfaces
    Vector3 massCentre;
    Vector3 rotationInertia;
    float upperLimitRadius;

    uint32_t unused;

    // Bitfields aren't portable
    // uint32_t maxDeviation : 8;
    // uint32_t byteSize : 24;

    int32_t offsetLedgetreeRoot;
    std::array<int32_t, 2> unused2;
    std::array<char, 4> id; // Should be IVPS
  };

  struct LedgeNode {
    int32_t rightNodeOffset;
    int32_t compactNodeOffset;
    Vector3 centre;
    float radius;
    std::array<uint8_t, 3> boxSizes;
    uint8_t unused;

    [[nodiscard]] bool isTerminal() const {
      return rightNodeOffset == 0;
    }
  };

  struct Ledge {
    int32_t pointOffset;
    int32_t boneIndex;

    uint32_t unused;

    // Bitfields aren't portable
    // uint32_t hasChildrenFlags : 2;
    // uint32_t isCompactFlag : 2;
    // uint32_t dummy : 4;
    // uint32_t sizeDiv16 : 24;

    uint16_t trianglesCount;
    int16_t unknown;
  };

  struct Edge {
    uint32_t data;

    // Bitfields aren't portable
    // uint32_t startPointIndex : 16;
    // uint32_t oppositePointIndex : 15;
    // uint32_t isVirtual : 1;

    [[nodiscard]] uint16_t getStartPointIndex() const {
      return data & 0b1111111111111111u;
    }
  };

  struct CompactTriangle {
    uint32_t data;

    // Bitfields aren't portable
    // uint32_t triangleIndex : 12;
    // uint32_t pierceIndex : 12;
    // uint32_t materialIndex : 7;
    // uint32_t isVirtual : 1;

    std::array<Edge, 3> edges;
  };
#pragma pack(pop)
}
