# MDLParser

Simple and modern library for parsing the Valve model formats.

## What's included

- Classes for parsing and abstracting the `MDL`, `VVD` and `VTX` file formats for the Source engine.
- Helper functions to simplify accessing the disparate but related data in all three files (see below).
- Enums, limits and structs with almost 100% coverage* of the formats.
- Runtime errors for issues when parsing the data due to corruption or a bug in the parser.

_*Much of the MDL data is not currently parsed or exposed. These can be used to either implement your own more complex
or specialised parser, or to easily extend - and hopefully PR - the `MdlParser::Mdl` class._

## Example

The following minimal example shows how you can easily and safely traverse the interconnected structure of the three
files using the accessor helpers:

```cpp
#include <mdlparser/mdlparser.hpp>

// Load from a file on disk, an API, or somewhere in memory
const auto mdlData = ...;
const auto vtxData = ...;
const auto vvdData = ...;

// Parse the data (you should wrap this in a try/catch)
const MdlParser::Mdl mdl(mdlData);
const MdlParser::Vtx vtx(vtxData, mdl.getChecksum());
const MdlParser::Vvd vvd(vvdData, mdl.getChecksum());

// You can construct all three with or without checksums,
// but passing one in will automatically check it against the header before parsing
// const Vtx vtx(vtxData);
// const Vvd vvd(vvdData);

// For each body part (body group) pair...
MdlParser::Accessors::iterateBodyParts(
  mdl,
  vtx,
  [&](const MdlParser::Mdl::BodyPart& mdlBodyPart, const MdlParser::Vtx::BodyPart& vtxBodyPart) {
    // For each model pair...
    MdlParser::Accessors::iterateModels(
      mdlBodyPart,
      vtxBodyPart,
      [&](const MdlParser::Mdl::Model& mdlModel, const MdlParser::Vtx::Model& vtxModel) {
        // Only using the highest LoD here
        const MdlParser::Vtx::ModelLod& vtxLod = vtxModel.levelOfDetails[0];

        // For each mesh pair...
        MdlParser::Accessors::iterateMeshes(
          mdlModel,
          vtxLod,
          [&](const MdlParser::Mdl::Mesh& mdlMesh, const MdlParser::Vtx::Mesh& vtxMesh) {
            // For each strip group...
            for (const MdlParser::Vtx::StripGroup& stripGroup : vtxMesh.stripGroups) {
              // For each vertex...
              MdlParser::Accessors::iterateVertices(
                vvd,
                mdlModel,
                mdlMesh,
                stripGroup,
                [&](
                  const MdlParser::Structs::Vtx::Vertex&,
                  const MdlParser::Structs::Vvd::Vertex& vvdVertex,
                  const MdlParser::Structs::Vector4D& tangent
                ) {
                  // ...
                }
              );
            }
          }
        );
      }
    );
  }
);
```
