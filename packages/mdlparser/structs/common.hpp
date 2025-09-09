#pragma once

#include <array>

namespace MdlParser::Structs {
  struct Vector2D {
    float x, y;
  };

  struct Vector {
    float x, y, z;
  };

  struct Vector4D {
    float x, y, z, w;
  };

  using Quaternion = Vector4D;
  using RadianEuler = Vector;

  struct Matrix3x4 {
    std::array<std::array<float, 4>, 3> m;

    std::array<float, 4>& operator[](int column) {
      return m[column];
    }
    const std::array<float, 4>& operator[](int column) const {
      return m[column];
    }
  };
}
