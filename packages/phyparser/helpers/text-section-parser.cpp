#include "text-section-parser.hpp"
#include <regex>
#include <sstream>
#include <source-parsers-shared/internal/offset-data-view.hpp>

namespace PhyParser {
  using namespace SourceParsers::Internal;

  namespace {
    struct KeyValueSegment {
      std::string type;
      std::vector<std::pair<std::string, std::string>> keyValues;
    };

    std::vector<KeyValueSegment> preParseTextSection(const std::string& raw) {
      std::vector<KeyValueSegment> parsed{};

      const std::regex headerRegex("(\\w+) \\{");
      const std::regex keyValueRegex("\"(\\w+)\" \"(.*)\"$");

      std::istringstream stream(raw);
      std::string line;
      std::smatch matches;
      while (std::getline(stream, line)) {
        if (!std::regex_match(line, matches, headerRegex)) {
          continue;
        }

        KeyValueSegment segment{};
        segment.type = matches[1].str();

        while (std::getline(stream, line)) {
          if (line[0] == '}') {
            break;
          }

          if (std::regex_match(line, matches, keyValueRegex)) {
            segment.keyValues.emplace_back(matches[1].str(), matches[2].str());
          }
        }

        parsed.push_back(std::move(segment));
      }

      return std::move(parsed);
    }
  }

  TextSection parseTextSection(const std::span<const std::byte> data) {
    const auto dataAsString = std::string(OffsetDataView(data).parseString(0, "Text section overflowed the file"));
    TextSection parsed{};

    for (const auto& segment : preParseTextSection(dataAsString)) {
      // TODO: Write something less awful than this
      if (segment.type == "ragdollconstraint") {
        TextSection::RagdollConstraint constraint{};

        for (const auto& [key, value] : segment.keyValues) {
          if (key == "parent") {
            constraint.parentIndex = std::stoi(value);
          } else if (key == "child") {
            constraint.childIndex = std::stoi(value);
          } else if (key == "xmin") {
            constraint.xMin = std::stof(value);
          } else if (key == "xmax") {
            constraint.xMax = std::stof(value);
          } else if (key == "xfriction") {
            constraint.xFriction = std::stof(value);
          } else if (key == "ymin") {
            constraint.yMin = std::stof(value);
          } else if (key == "ymax") {
            constraint.yMax = std::stof(value);
          } else if (key == "yfriction") {
            constraint.yFriction = std::stof(value);
          } else if (key == "zmin") {
            constraint.zMin = std::stof(value);
          } else if (key == "zmax") {
            constraint.zMax = std::stof(value);
          } else if (key == "zfriction") {
            constraint.zFriction = std::stof(value);
          }
        }

        parsed.ragdollConstraints.push_back(constraint);
      } else if (segment.type == "solid") {
        TextSection::Solid solid{};

        for (const auto& [key, value] : segment.keyValues) {
          if (key == "index") {
            solid.index = std::stoi(value);
          } else if (key == "name") {
            solid.name = value;
          } else if (key == "parent") {
            solid.parentName = value;
          } else if (key == "mass") {
            solid.mass = std::stof(value);
          } else if (key == "surfaceprop") {
            solid.surfaceProperties = value;
          } else if (key == "damping") {
            solid.damping = std::stof(value);
          } else if (key == "rotdamping") {
            solid.rotationDamping = std::stof(value);
          } else if (key == "inertia") {
            solid.inertia = std::stof(value);
          } else if (key == "volume") {
            solid.volume = std::stof(value);
          }
        }

        parsed.solids.emplace(solid.index, solid);
      }
    }

    return std::move(parsed);
  }
}
