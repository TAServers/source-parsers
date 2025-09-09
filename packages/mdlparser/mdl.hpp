#pragma once

#include <optional>
#include <span>
#include <string>
#include <vector>
#include "structs/mdl.hpp"

namespace MdlParser {
  /**
   * Parses a .mdl file from a buffer into an easier to traverse structure using STL containers.
   */
  class Mdl {
  public:
    /**
     * A collection of primitives with a common set of vertices, indices and material.
     */
    struct Mesh {
      /**
       * Column of the skin lookup table to use for this mesh.
       * @code
       * mdl.getSkinLookupTable()[skinFamily][mesh.material]
       * @endcode
       */
      int32_t material;

      /**
       * Offset to be added to the parent Model's offsets into the VVD's vertex and tangent arrays.
       * @remark Unlike Model, each Mesh has only one offset for both vertices and tangents.
       */
      int32_t vertexOffset;

      /**
       * Number of vertices and tangents in this mesh.
       */
      int32_t vertexCount;
    };

    /**
     * A logical grouping of meshes that can be toggled between in a body part.
     */
    struct Model {
      /**
       * The meshes which make up this model
       */
      std::vector<Mesh> meshes;

      /**
       * Offset into the VVD's vertex array.
       */
      int32_t vertexOffset;

      /**
       * Offset into the VVD's tangent array.
       */
      int32_t tangentsOffset;

      /**
       * Number of vertices and tangents in this model.
       */
      int32_t vertexCount;
    };

    /**
     * A body part (or body group) is a group of models of which exactly one will be displayed at a given time.
     * @remark This is used for toggling GMan's briefcase, selecting the handle type of a door, and much more.
     */
    struct BodyPart {
      /**
       * Human readable name for this body part.
       */
      std::string name;

      /**
       * The models which can be toggled between.
       */
      std::vector<Model> models;
    };

    /**
     * A bone forming part of the model's skeleton.
     */
    struct Bone {
      /**
       * The human readable name of this bone.
       */
      std::string name;

      /**
       * Index of this bone's parent.
       */
      int32_t parent;

      Structs::Vector position;
      Structs::Quaternion orientation;
      Structs::RadianEuler orientationEuler;
      Structs::Vector positionScale;
      Structs::Vector orientationScale;

      /**
       * Matrix to translate vertices from model space to bone space for skinning.
       */
      Structs::Matrix3x4 poseToBone;

      /**
       * Bitflags describing this bone. An enum is not currently provided with the possible values and their meanings.
       */
      int32_t flags;
    };

    /**
     * A reference to a VTF file used by the model.
     */
    struct Texture {
      /**
       * The filename of the VTF file only.
       * To determine the actual path to the texture, you must iterate through the texture directories returned by getTextureDirectories().
       */
      std::string name;
      int32_t flags;
    };

    /**
     * Parses a .mdl file contained in the given buffer into an easier to use and more modern structure.
     * No ownership of the data is taken but all contents are copied into new structs, so the Mdl instance may outlive data.
     *
     * @param data
     * @param checksum Optional checksum to validate against the header's
     */
    explicit Mdl(
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
     * Gets the list of body parts (body groups) that make up the model.
     * @return List of body parts.
     */
    [[nodiscard]] const std::vector<BodyPart>& getBodyParts() const;

    /**
     * Gets the list of directories (relative to /materials) which contain the textures used by the model.
     * As each texture only includes its filename, you must search through this list of directories to find
     * the actual path to the image asset.
     *
     * @return List of paths relative to /materials.
     */
    [[nodiscard]] const std::vector<std::string>& getTextureDirectories() const;

    /**
     * Gets the list of textures used by this model.
     * As each texture only stores its filename, you must use getTextureDirectories() to determine the actual path.
     * @return List of textures.
     */
    [[nodiscard]] const std::vector<Texture>& getTextures() const;

    /**
     * Gets the skin lookup table as a row-major 2-dimensional array.
     * To map from a material index to a texture index for a given skin, simply do:
     * @code
     * mdl.getSkinLookupTable()[skinFamily][mesh.material]
     * @endcode
     * @return Skin lookup table as a 2D array.
     */
    [[nodiscard]] const std::vector<std::vector<int16_t>>& getSkinLookupTable() const;

    /**
     * Gets the list of bones in the model.
     * @remarks Models which you wouldn't expect to be rigged may still have a root bone,
     * and that bone may be rotated relative to the model's overall origin (usually 90 degrees around the z axis).
     * @return List of bones.
     */
    [[nodiscard]] const std::vector<Bone>& getBones() const;

  private:
    Structs::Mdl::Header header{};
    std::optional<Structs::Mdl::Header2> header2;

    std::vector<BodyPart> bodyParts;

    std::vector<std::string> textureDirectories;
    std::vector<Texture> textures;
    std::vector<std::vector<int16_t>> skins;

    std::vector<Bone> bones;
  };
}
