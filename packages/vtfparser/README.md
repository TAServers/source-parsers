# VTFParser

Simple and modern library for parsing the Valve Texture Format.

## What's Included

- A class for parsing and abstracting the VTF file format.
- Enums, limits and structs for most of the file format.
  - Unlike older versions of the library, structs are not provided for each of the possible pixel formats. Most if not
    all of the formats are supported by each major graphics API.

## Example

The following minimal example shows how you can parse and access the VTF high-res image data:

```cpp
#include <vtfparser/vtfparser.hpp>

// Load from a file on disk, an API, or somewhere in memory
const auto vtfData = ...;

// Parse the data (you should wrap this in a try/catch)
const VtfParser::Vtf vtf(vtfData);

// Read data (exact approach will depend on your use-case)
const auto imageData = vtf.getHighResImageData();
for (uint32_t mipLevel = 0; mipLevel < vtf.getMipLevels(); mipLevel++) {
  for (uint32_t frame = 0; frame < vtf.getFrames(); frame++) {
    for (uint32_t face = 0; face < vtf.getFaces(); face++) {
      const auto offset = vtf.getImageSliceOffset(mipLevel, frame, face);
      // imageData[offset]
    }
  }
}
```
