#pragma once

#include <functional>
#include "mdl.hpp"
#include "vtx.hpp"
#include "vvd.hpp"

/**
 * A collection of helper functions to ease traversing the MDL, VTX and VVD structures together.
 */
namespace MdlParser::Accessors {
  /**
   * Iterates over the pairs of body parts in the MDL and VTX data, calling iteratee with each pair.
   * @param mdl MDL data.
   * @param vtx VTX data.
   * @param iteratee Function to be called for each pair, taking the MDL body part followed by the VTX body part.
   */
  void iterateBodyParts(
    const Mdl& mdl,
    const Vtx& vtx,
    const std::function<void(const Mdl::BodyPart &, const Vtx::BodyPart &)>& iteratee
  );

  /**
   * Iterates over the pairs of models in the MDL and VTX data, calling iteratee with each pair.
   * @param mdlBodyPart Body part in the MDL data.
   * @param vtxBodyPart Body part in the VTX data.
   * @param iteratee Function to be called for each pair, taking the MDL model followed by the VTX model.
   */
  void iterateModels(
    const Mdl::BodyPart& mdlBodyPart,
    const Vtx::BodyPart& vtxBodyPart,
    const std::function<void(const Mdl::Model &, const Vtx::Model &)>& iteratee
  );

  /**
   * Iterates over the pairs of meshes in the MDL and VTX data, calling iteratee with each pair.
   * @param mdlModel Model in the MDL data.
   * @param vtxModel Model in the VTX data.
   * @param iteratee Function to be called for each pair, taking the MDL mesh followed by the VTX mesh.
   */
  void iterateMeshes(
    const Mdl::Model& mdlModel,
    const Vtx::ModelLod& vtxModel,
    const std::function<void(const Mdl::Mesh &, const Vtx::Mesh &)>& iteratee
  );

  /**
   * Iterates over the vertex data for the given strip group, calling iteratee with the VTX and VVD vertex data plus the tangent.
   * @param vvd Parsed VVD containing per-vertex data.
   * @param model Parsed model from the MDL containing offsets into the VVD.
   * @param mesh Parsed mesh from the MDL containing offsets into the VVD.
   * @param stripGroup VTX strip group to read vertex data from.
   * @param iteratee Function to be called for each vertex, taking the VTX vertex, VVD vertex and tangent (in that order).
   */
  void iterateVertices(
    const Vvd& vvd,
    const Mdl::Model& model,
    const Mdl::Mesh& mesh,
    const Vtx::StripGroup& stripGroup,
    const std::function<void(const Structs::Vtx::Vertex &, const Structs::Vvd::Vertex &, const Structs::Vector4D &)>&
    iteratee
  );
}
