#pragma once

#include <cstdint>

namespace PhyParser::Enums {
  enum class ModelType : int16_t { // NOLINT(*-enum-size)
    IVPCompactSurface = 0,
    IVPMOPP = 1,
    IVPBall = 2,
    IVPVirtual = 3,
    Unknown = 4,
  };
}
