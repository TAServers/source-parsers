cmake_minimum_required(VERSION 3.28)

# Traditionally, you shouldn't GLOB source files as it breaks changed file detection.
# However, the "new" CONFIGURE_DEPENDS parameter solves this for most cases with minimal bulid-time overhead
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.cpp")

add_library(SourceParsersShared STATIC ${sources})
