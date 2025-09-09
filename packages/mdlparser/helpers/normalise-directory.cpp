#include "normalise-directory.hpp"
#include <regex>

namespace MdlParser {

  std::string getNormalisedDirectory(const std::string& raw) {
    return std::regex_replace(raw, std::regex("\\\\"), "/");
  }
}
