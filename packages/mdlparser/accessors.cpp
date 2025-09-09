#include "accessors.hpp"
#include <source-parsers-shared/errors.hpp>

namespace MdlParser::Accessors {
  namespace {
    using namespace SourceParsers::Errors;

    template<typename T1, typename T2>
    void iteratePairs(
      const std::vector<T1>& first,
      const std::vector<T2>& second,
      const std::function<void(const T1&, const T2&)>& iteratee
    ) {
      if (first.size() != second.size()) {
        throw OutOfBoundsAccess("Failed to iterate through pairs. Lengths do not match");
      }

      for (size_t i = 0; i < first.size(); i++) {
        iteratee(first[i], second[i]);
      }
    }
  }

  void iterateBodyParts(
    const Mdl& mdl,
    const Vtx& vtx,
    const std::function<void(const Mdl::BodyPart &, const Vtx::BodyPart &)>& iteratee
  ) {
    iteratePairs(mdl.getBodyParts(), vtx.getBodyParts(), iteratee);
  }

  void iterateModels(
    const Mdl::BodyPart& mdlBodyPart,
    const Vtx::BodyPart& vtxBodyPart,
    const std::function<void(const Mdl::Model &, const Vtx::Model &)>& iteratee
  ) {
    iteratePairs(mdlBodyPart.models, vtxBodyPart.models, iteratee);
  }

  void iterateMeshes(
    const Mdl::Model& mdlModel,
    const Vtx::ModelLod& vtxModel,
    const std::function<void(const Mdl::Mesh &, const Vtx::Mesh &)>& iteratee
  ) {
    iteratePairs(mdlModel.meshes, vtxModel.meshes, iteratee);
  }

  void iterateVertices(
    const Vvd& vvd,
    const Mdl::Model& model,
    const Mdl::Mesh& mesh,
    const Vtx::StripGroup& stripGroup,
    const std::function<void(const Structs::Vtx::Vertex &, const Structs::Vvd::Vertex &, const Structs::Vector4D &)>&
    iteratee
  ) {
    const auto& vvdVertices = vvd.getVertices();
    const auto& vvdTangents = vvd.getTangents();

    for (const auto& vtxVertex : stripGroup.vertices) {
      const auto vvdVertexIndex = model.vertexOffset + mesh.vertexOffset + vtxVertex.origMeshVertId;
      const auto vvdTangentIndex = model.tangentsOffset + mesh.vertexOffset + vtxVertex.origMeshVertId;

      iteratee(vtxVertex, vvdVertices[vvdVertexIndex], vvdTangents[vvdTangentIndex]);
    }
  }
}
