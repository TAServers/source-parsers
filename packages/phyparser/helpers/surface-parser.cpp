#include "surface-parser.hpp"
#include <map>
#include <ranges>
#include <stack>
#include <source-parsers-shared/errors.hpp>
#include <source-parsers-shared/internal/offset-data-view.hpp>

namespace PhyParser {
  using namespace SourceParsers::Errors;
  using namespace SourceParsers::Internal;

  namespace {
    using namespace Structs;

    uint16_t remapIndex(const Edge& edge, std::map<uint16_t, uint16_t>& remappedIndices, uint16_t& maxIndex) {
      const auto index = edge.getStartPointIndex();
      if (remappedIndices.contains(index)) {
        return remappedIndices.at(index);
      }

      maxIndex = std::max(maxIndex, index);

      const auto remappedIndex = remappedIndices.size();
      remappedIndices.emplace(index, remappedIndex);

      return remappedIndex;
    }

    Solid parseLedge(const Ledge& ledge, const Vector3& centreOfMass, const OffsetDataView& data) {
      const auto triangles = data.parseStructArray<CompactTriangle>(
        sizeof(Ledge),
        ledge.trianglesCount,
        "Failed to parse triangle array"
      );

      std::vector<uint16_t> indices;
      indices.reserve(static_cast<size_t>(ledge.trianglesCount) * 3);
      std::map<uint16_t, uint16_t> remappedIndices;

      uint16_t maxVertexIndex = 0;
      for (const auto& triangle : triangles) {
        for (auto edgeIndex = 0; edgeIndex < 3; edgeIndex++) {
          indices.push_back(remapIndex(triangle.edges.at(edgeIndex), remappedIndices, maxVertexIndex));
        }
      }

      std::vector<Vector4> vertices;
      vertices.resize(remappedIndices.size());
      const auto sharedVertexBuffer = data.parseStructArray<Vector4>(
        ledge.pointOffset,
        maxVertexIndex + 1,
        "Failed to parse vertex array"
      );

      for (const auto [sourceIndex, destIndex] : remappedIndices) {
        vertices[destIndex] = sharedVertexBuffer[sourceIndex];
      }

      return {
        .vertices = std::move(vertices),
        .indices = std::move(indices),
        .centreOfMass = centreOfMass,
        .boneIndex = ledge.boneIndex,
      };
    }

    std::vector<Solid> parseMopp(const OffsetDataView& /*data*/) {
      throw std::runtime_error("Not implemented");
    }

    std::vector<Solid> parseCompactSurface(const OffsetDataView& data) {
      const auto [surfaceHeader, headerOffset] =
        data.parseStructWithOffset<CompactSurfaceHeader>(0, "Failed to parse compact surface header");

      const auto rootNode =
        headerOffset + offsetof(CompactSurfaceHeader, massCentre) + surfaceHeader.offsetLedgetreeRoot;

      const auto nodeData = data.withAbsoluteOffset(0);

      std::vector<Solid> solids;
      std::stack<size_t> nodeOffsets;
      nodeOffsets.push(rootNode);

      while (!nodeOffsets.empty()) {
        auto [node, nodeOffset] = nodeData.parseStructWithOffset<LedgeNode>(
          nodeOffsets.top(),
          "Failed to parse ledge node"
        );
        nodeOffsets.pop();

        if (node.isTerminal()) {
          const auto [ledge, ledgeOffset] =
            nodeData.parseStructWithOffset<Ledge>(nodeOffset + node.compactNodeOffset, "Failed to parse ledge");

          solids.push_back(parseLedge(ledge, surfaceHeader.massCentre, data.withAbsoluteOffset(ledgeOffset)));
        } else {
          nodeOffsets.push(nodeOffset + node.rightNodeOffset);
          nodeOffsets.push(nodeOffset + sizeof(LedgeNode));
        }
      }

      return std::move(solids);
    }

    std::vector<Solid> parseSurface(const SurfaceHeader& surfaceHeader, const OffsetDataView& data) {
      switch (surfaceHeader.modelType) {
        case Enums::ModelType::IVPCompactSurface:
          return parseCompactSurface(data);
        case Enums::ModelType::IVPMOPP:
          return parseMopp(data);
        case Enums::ModelType::IVPBall:
          throw InvalidBody("Unsupported surface model type IVPBall");
        case Enums::ModelType::IVPVirtual:
          throw InvalidBody("Unsupported surface model type IVPVirtual");
        default:
          throw InvalidBody("Unrecognised surface model type");
      }
    }
  }

  std::pair<std::vector<Solid>, size_t> parseSurfaces(std::span<std::byte const> data, size_t solidCount) {
    std::vector<Solid> solids;
    solids.reserve(solidCount);

    const auto dataView = OffsetDataView(data);
    size_t offset = 0;
    for (int i = 0; i < solidCount; i++) {
      const auto surfaceHeader = dataView.parseStruct<SurfaceHeader>(offset, "Failed to parse surface header");

      auto solidsForSurface = parseSurface(surfaceHeader, dataView.withAbsoluteOffset(offset + sizeof(SurfaceHeader)));
      solids.insert(
        solids.end(),
        std::make_move_iterator(solidsForSurface.begin()),
        std::make_move_iterator(solidsForSurface.end())
      );

      offset += surfaceHeader.size + sizeof(SurfaceHeader::size);
    }

    return std::make_pair(std::move(solids), offset);
  }
}
