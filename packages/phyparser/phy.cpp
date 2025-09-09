#include "phy.hpp"
#include <algorithm>
#include <ranges>
#include <source-parsers-shared/errors.hpp>
#include <source-parsers-shared/internal/offset-data-view.hpp>

namespace PhyParser {
  using namespace SourceParsers::Errors;
  using namespace SourceParsers::Internal;
  using namespace Enums;
  using namespace Structs;

  Phy::Phy(const std::span<const std::byte> data, const std::optional<int64_t>& checksum) {
    const OffsetDataView dataView(data);
    header = dataView.parseStruct<Header>(0, "Failed to parse PHY header");

    if (checksum.has_value() && header.checksum != checksum.value()) {
      throw InvalidChecksum("PHY checksum does not match");
    }

    auto [solids, solidDataSize] = parseSurfaces(data.subspan(header.size), header.solidCount);
    this->solids = std::move(solids);

    textSection = parseTextSection(data.subspan(header.size + solidDataSize));
  }

  int64_t Phy::getChecksum() const {
    return header.checksum;
  }

  const std::vector<Solid>& Phy::getSolids() const {
    return solids;
  }

  const TextSection& Phy::getTextSection() const {
    return textSection;
  }
}
