cmake_minimum_required(VERSION 3.23)

project("BSPParser" CXX)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(packages/shared)
include(cmake/RegisterPublicPackage.cmake)

RegisterPublicPackage(bspparser)
