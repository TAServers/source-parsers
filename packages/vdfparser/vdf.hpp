#pragma once

#include "keyvalue.hpp"

/**
 * Abstracts parsing and accessing the contents of the Valve Data File (VDF) format.
 */
namespace VdfParser {
  /**
   * Parses a key-value structure from a string containing the raw VDF data.
   * @param raw Raw VDF key-value data
   * @return Parsed key-values structure
   */
  KeyValue fromString(const std::string& raw);
}
