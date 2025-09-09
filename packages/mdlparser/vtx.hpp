#pragma once

#include <optional>
#include <span>
#include <string>
#include <vector>
#include "enums.hpp"
#include "structs/vtx.hpp"

namespace MdlParser {
  /**
   * Parses a .vtx file from a buffer into an easier to traverse structure using STL containers.
   */
  class Vtx {
  public:
    /**
     * A primitive storing offsets into the parent strip group's vertices and indices.
     */
    struct Strip {
      /**
       * The number of vertices in the strip group which make up this strip.
       */
      int32_t verticesCount;
      /**
       * The offset of the strip's first vertex in the strip group's vertices.
       */
      int32_t verticesOffset;

      /**
       * The number of indices in the strip group which make up this strip.
       */
      int32_t indicesCount;
      /**
       * The offset of the strip's first index in the strip group's indices.
       */
      int32_t indicesOffset;

      /**
       * Bitflags describing the strip.
       */
      Enums::Vtx::StripFlags flags;
    };

    /**
     * A collection of primitives (strips) with common vertices and indices.
     */
    struct StripGroup {
      /**
       * The vertices used by the strips in this group.
       * @remarks The majority of the vertex data is stored in the .vvd file, with the vertices in here mostly just pointing to that data.
       */
      std::vector<Structs::Vtx::Vertex> vertices;

      /**
       * The indices used by the strips in this group.
       * Each index is an offset into the strip group's vertices.
       */
      std::vector<uint16_t> indices;

      /**
       * The strips (primitives) within this group.
       */
      std::vector<Strip> strips;

      /**
       * Bitflags describing this strip group.
       */
      Enums::Vtx::StripGroupFlags flags;
    };

    /**
     * A collection of primitives grouped to be more optimised for legacy rendering APIs.
     */
    struct Mesh {
      /**
       * The groups which make up this mesh.
       */
      std::vector<StripGroup> stripGroups;

      /**
       * Bitflags describing this mesh.
       */
      Enums::Vtx::MeshFlags flags;
    };

    /**
     * A collection of meshes to be displayed at a certain distance to the viewer.
     */
    struct ModelLod {
      /**
       * The meshes that make up this level of detail.
       */
      std::vector<Mesh> meshes;

      /**
       * The point (distance?) at which you should switch to (from?) this level of detail (in hammer units?).
       */
      float switchPoint;
    };

    /**
     * A logical grouping of meshes that can be toggled between in a body part.
     */
    struct Model {
      /**
       * The level of details available for this model (with 0 being the highest).
       */
      std::vector<ModelLod> levelOfDetails;
    };

    /**
     * A body part (or body group) is a group of models of which exactly one will be displayed at a given time.
     * @remark This is used for toggling GMan's briefcase, selecting the handle type of a door, and much more.
     */
    struct BodyPart {
      /**
       * The models which can be toggled between.
       */
      std::vector<Model> models;
    };

    struct MaterialReplacement {
      int16_t replacementId;
      std::string replacementName;
    };

    /**
     * Parses a .vtx file contained in the given buffer into an easier to use and more modern structure.
     * No ownership of the data is taken but all contents are copied into new structs, so the Vtx can safely outlive data.
     *
     * @param data
     * @param checksum Optional checksum to validate against the header's
     */
    explicit Vtx(
      std::span<const std::byte> data,
      const std::optional<int32_t>& checksum = std::nullopt
    );

    /**
     * Gets the checksum shared by the MDL, VTX and VVD from the header.
     * @remarks Can be used to loosely verify that a collection of MDL, VTX and VVD files were compiled from the same asset.
     * @return int32_t checksum
     */
    [[nodiscard]] int32_t getChecksum() const;

    /**
     * Gets the material replacements for a given level of detail.
     * @param lod
     * @return The material replacements list.
     */
    [[nodiscard]] const std::vector<MaterialReplacement>& getMaterialReplacements(const int lod) const;

    /**
     * Gets the body parts (body groups) which make up this model.
     * @return
     */
    [[nodiscard]] const std::vector<BodyPart>& getBodyParts() const;

  private:
    Structs::Vtx::Header header{};
    std::vector<BodyPart> bodyParts;
    std::vector<std::vector<MaterialReplacement>> materialReplacementsByLod;
  };
}
