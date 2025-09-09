#include "vtx.hpp"
#include "structs/vtx.hpp"
#include <cstdint>
#include <optional>
#include <source-parsers-shared/errors.hpp>
#include <source-parsers-shared/internal/offset-data-view.hpp>

namespace MdlParser {
  using Structs::Vtx::Header;
  using namespace SourceParsers::Errors;
  using namespace SourceParsers::Internal;

  namespace {
    Vtx::Strip parseStrip(const Structs::Vtx::Strip& strip) {
      return {
        .verticesCount = strip.numVerts,
        .verticesOffset = strip.vertOffset,
        .indicesCount = strip.numIndices,
        .indicesOffset = strip.indexOffset,
        .flags = strip.flags,
      };
    }

    Vtx::StripGroup parseStripGroup(const OffsetDataView& data, const Structs::Vtx::StripGroup& stripGroup) {
      std::vector<Vtx::Strip> strips;
      strips.reserve(stripGroup.numStrips);

      for (const auto& [strip, _] : data.parseStructArrayWithOffsets<Structs::Vtx::Strip>(
             stripGroup.stripOffset,
             stripGroup.numStrips,
             "Failed to parse VTX strip array"
           )) {
        checkBounds(
          strip.vertOffset,
          strip.numVerts,
          stripGroup.numVerts,
          "VTX strip accesses outside strip group vertex data"
        );
        checkBounds(
          strip.indexOffset,
          strip.numIndices,
          stripGroup.numIndices,
          "VTX strip accesses outside strip group index data"
        );

        strips.push_back(parseStrip(strip));
      }

      const auto vertices = data.parseStructArray<Structs::Vtx::Vertex>(
        stripGroup.vertOffset,
        stripGroup.numVerts,
        "Failed to parse VTX vertex array"
      );
      const auto indices = data.parseStructArray<uint16_t>(
        stripGroup.indexOffset,
        stripGroup.numIndices,
        "Failed to parse VTX index array"
      );

      return {
        .vertices = std::vector(vertices.begin(), vertices.end()),
        .indices = std::vector(indices.begin(), indices.end()),
        .strips = std::move(strips),
        .flags = stripGroup.flags,
      };
    }

    Vtx::Mesh parseMesh(const OffsetDataView& data, const Structs::Vtx::Mesh& mesh) {
      std::vector<Vtx::StripGroup> stripGroups;
      stripGroups.reserve(mesh.numStripGroups);

      for (const auto& [stripGroup, offset] : data.parseStructArrayWithOffsets<Structs::Vtx::StripGroup>(
             mesh.stripGroupHeaderOffset,
             mesh.numStripGroups,
             "Failed to parse VTX strip group array"
           )) {
        stripGroups.push_back(parseStripGroup(data.withAbsoluteOffset(offset), stripGroup));
      }

      return { .stripGroups = stripGroups, .flags = mesh.flags };
    }

    Vtx::ModelLod parseModelLod(const OffsetDataView& data, const Structs::Vtx::ModelLoD& lod) {
      std::vector<Vtx::Mesh> meshes;
      meshes.reserve(lod.numMeshes);

      for (const auto& [mesh, offset] :
           data.parseStructArrayWithOffsets<Structs::Vtx::Mesh>(
             lod.meshOffset,
             lod.numMeshes,
             "Failed to parse VTX mesh array"
           )) {
        meshes.push_back(parseMesh(data.withAbsoluteOffset(offset), mesh));
      }

      return { .meshes = std::move(meshes), .switchPoint = lod.switchPoint };
    }

    Vtx::Model parseModel(const OffsetDataView& data, const Structs::Vtx::Model& model) {
      std::vector<Vtx::ModelLod> lods;
      lods.reserve(model.numLoDs);

      for (const auto& [lod, offset] : data.parseStructArrayWithOffsets<Structs::Vtx::ModelLoD>(
             model.lodOffset,
             model.numLoDs,
             "Failed to parse VTX model LoD array"
           )) {
        lods.push_back(parseModelLod(data.withAbsoluteOffset(offset), lod));
      }

      return { .levelOfDetails = std::move(lods) };
    }

    Vtx::BodyPart parseBodyPart(
      const OffsetDataView& data,
      const Structs::Vtx::BodyPart& bodyPart,
      const int32_t expectedLods
    ) {
      std::vector<Vtx::Model> models;
      models.reserve(bodyPart.numModels);

      for (const auto& [model, offset] : data.parseStructArrayWithOffsets<Structs::Vtx::Model>(
             bodyPart.modelOffset,
             bodyPart.numModels,
             "Failed to parse VTX model array"
           )) {
        if (model.numLoDs != expectedLods) {
          throw InvalidBody("VTX model LoD count does not match header");
        }

        models.push_back(parseModel(data.withAbsoluteOffset(offset), model));
      }

      return { .models = std::move(models) };
    }
  }

  Vtx::Vtx(const std::span<const std::byte> data, const std::optional<int32_t>& checksum) {
    const OffsetDataView dataView(data);
    header = dataView.parseStruct<Header>(0, "Failed to parse VTX header");

    if (header.version != Header::SUPPORTED_VERSION) {
      throw UnsupportedVersion("VTX version is unsupported");
    }
    if (checksum.has_value() && header.checksum != checksum.value()) {
      throw InvalidChecksum("VTX checksum does not match");
    }

    bodyParts.reserve(header.numBodyParts);
    for (const auto& [bodyPart, offset] : dataView.parseStructArrayWithOffsets<Structs::Vtx::BodyPart>(
           header.bodyPartOffset,
           header.numBodyParts,
           "Failed to parse VTX body part array"
         )) {
      bodyParts.push_back(parseBodyPart(dataView.withAbsoluteOffset(offset), bodyPart, header.numLoDs));
    }

    materialReplacementsByLod.reserve(header.numLoDs);
    for (const auto& [replacementList, replacementListOffset] :
         dataView.parseStructArrayWithOffsets<Structs::Vtx::MaterialReplacementList>(
           header.materialReplacementListOffset,
           header.numLoDs,
           "Failed to parse VTX material replacement lists"
         )) {
      std::vector<MaterialReplacement> replacements;
      replacements.reserve(replacementList.replacementCount);

      for (const auto& [replacement, replacementOffset] : dataView.withAbsoluteOffset(replacementListOffset)
           .parseStructArrayWithOffsets<Structs::Vtx::MaterialReplacement>(
             replacementList.replacementOffset,
             replacementList.replacementCount,
             "Failed to parse VTX material replacements"
           )) {
        replacements.push_back(
          {
            .replacementId = replacement.materialId,
            .replacementName = std::string(
              dataView.withAbsoluteOffset(replacementOffset).parseString(
                replacement.replacementMaterialNameOffset,
                "Failed to parse VTX material replacement name"
              )
            ),
          }
        );
      }

      materialReplacementsByLod.push_back(std::move(replacements));
    }
  }

  int32_t Vtx::getChecksum() const {
    return header.checksum;
  }

  const std::vector<Vtx::MaterialReplacement>& Vtx::getMaterialReplacements(const int lod) const {
    checkBounds(lod, 1, materialReplacementsByLod.size(), "Level of detail is outside range");
    return materialReplacementsByLod[lod];
  }

  const std::vector<Vtx::BodyPart>& Vtx::getBodyParts() const {
    return bodyParts;
  }
}
