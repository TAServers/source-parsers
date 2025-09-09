#pragma once

// Doc comments for namespaces are required for Doxygen to register them

/**
 * Root namespace containing all BspParser classes and functions.
 */
namespace BspParser {
  /**
   * Accessor helpers for iterating contents of the BSP.
   */
  namespace Accessors {}

  /**
   * File format enums.
   */
  namespace Enums {}

  /**
   * File format structs.
   */
  namespace Structs {}

  /**
   * Zip file parsing.
   */
  namespace Zip {}
}

#include "bsp.hpp"
#include "accessors/face-accessors.hpp"
#include "accessors/prop-accessors.hpp"
#include "accessors/texture-accessors.hpp"
