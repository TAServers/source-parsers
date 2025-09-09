#pragma once

#include <optional>
#include <span>
#include <vector>
#include "helpers/surface-parser.hpp"
#include "helpers/text-section-parser.hpp"
#include "structs/phy.hpp"

namespace PhyParser {
  class Phy {
  public:
    explicit Phy(std::span<const std::byte> data, const std::optional<int64_t>& checksum = std::nullopt);

    [[nodiscard]] int64_t getChecksum() const;

    [[nodiscard]] const std::vector<Solid>& getSolids() const;

    [[nodiscard]] const TextSection& getTextSection() const;

  private:
    Structs::Header header{};
    std::vector<Solid> solids;
    TextSection textSection;
  };
}
