#pragma once

#include <cstdint>
#include <span>
#include <utility>
#include <vector>
#include "../structs/phy.hpp"

namespace PhyParser {
  struct Solid {
    std::vector<Structs::Vector4> vertices;
    std::vector<uint16_t> indices;
    Structs::Vector3 centreOfMass;
    int32_t boneIndex;
  };

  /**
   * Parses solids from each surface in the data.
   * @remarks Returns the size in bytes read, which you can use to read the text section immediately following the surfaces in both .phy and .bsp files.
   * @param data Bytes containing the surface headers and data, starting with the first surface header.
   * @param count Number of solids total across all surfaces.
   * @return List of solids and the total number of bytes that were read.
   */
  std::pair<std::vector<Solid>, size_t> parseSurfaces(std::span<const std::byte> data, size_t count);
}
