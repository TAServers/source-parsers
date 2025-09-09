#pragma once

#include <optional>
#include <span>
#include <vector>
#include "structs/vvd.hpp"

namespace MdlParser {
  /**
   * Parses a .vtx file from a buffer into an easier to traverse structure using STL containers.
   */
  class Vvd {
  public:
    /**
     * Parses a .vvd file contained in the given buffer into an easier to use and more modern structure.
     * No ownership of the data is taken as all contents are copied into new structs.
     *
     * @param data
     * @param checksum Optional checksum to validate against the header's.
     */
    explicit Vvd(
      std::span<const std::byte> data,
      const std::optional<int32_t>& checksum = std::nullopt
    );

    /**
     * Gets the checksum shared by the MDL, VTX and VVD from the header.
     * @remarks Can be used to loosely verify that a collection of MDL, VTX and VVD files were compiled from the same asset.
     * @return int32_t checksum.
     */
    [[nodiscard]] int32_t getChecksum() const;

    /**
     * Gets the list of vertices in this VVD.
     * @return List of vertices.
     */
    [[nodiscard]] const std::vector<Structs::Vvd::Vertex>& getVertices() const;

    /**
     * Gets the list of tangents in this VVD.
     * @remarks These are stored separately from the primary vertex data, and are indexed using different offsets in the MDL file.
     * @return List of tangents.
     */
    [[nodiscard]] const std::vector<Structs::Vector4D>& getTangents() const;

    /**
     * Gets the number of levels of detail (LoDs) that should be present in the model.
     * @return Number of levels.
     */
    [[nodiscard]] int32_t getLevelsOfDetail() const;

  private:
    Structs::Vvd::Header header{};
    std::vector<Structs::Vvd::Vertex> vertices;
    std::vector<Structs::Vector4D> tangents;
  };
}
