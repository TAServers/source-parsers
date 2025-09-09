cmake_minimum_required(VERSION 3.28)

project("SourceParsers" CXX)
set(CMAKE_CXX_STANDARD 20)

include(cmake/RegisterPublicPackage.cmake)

RegisterPublicPackage(bspparser)
RegisterPublicPackage(phyparser)
RegisterPublicPackage(vdfparser)
RegisterPublicPackage(vpkparser)
RegisterPublicPackage(vtfparser)
