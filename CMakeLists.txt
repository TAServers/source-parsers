cmake_minimum_required(VERSION 3.23)

project("SourceParsers" CXX)
set(CMAKE_CXX_STANDARD 20)

include(cmake/RegisterPublicPackage.cmake)

RegisterPublicPackage(source-parsers-shared)
find_package(source-parsers-shared CONFIG REQUIRED)

RegisterPublicPackage(bspparser)
