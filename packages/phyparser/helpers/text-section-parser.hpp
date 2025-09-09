#pragma once

#include <cstdint>
#include <map>
#include <span>
#include <string>
#include <vector>

namespace PhyParser {
  struct TextSection {
    struct Solid {
      uint32_t index = 0;
      std::string name;
      std::string parentName;
      float mass = 0;
      std::string surfaceProperties;
      float damping = 0;
      float rotationDamping = 0;
      float inertia = 0;
      float volume = 0;
    };

    struct RagdollConstraint {
      uint32_t parentIndex = 0;
      uint32_t childIndex = 0;

      float xMin = -180;
      float xMax = 180;
      float xFriction = 0;

      float yMin = -180;
      float yMax = 180;
      float yFriction = 0;

      float zMin = -180;
      float zMax = 180;
      float zFriction = 0;
    };

    struct CollisionRules {
      std::vector<std::pair<uint32_t, uint32_t>> collisionPairs;
    };

    struct AnimatedFriction {
      float min = 0;
      float max = 0;
      float timeIn = 0;
      float timeOut = 0;
      float timeHold = 0;
    };

    struct EditParams {
      std::string rootName;
      float totalMass = 0;
      std::vector<std::string> jointMerges;
    };

    std::map<uint32_t, Solid> solids;
    std::vector<RagdollConstraint> ragdollConstraints;
    CollisionRules collisionRules;
    AnimatedFriction animatedFriction;
    EditParams editParams;
  };

  TextSection parseTextSection(std::span<const std::byte> data);
}
