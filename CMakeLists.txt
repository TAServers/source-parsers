cmake_minimum_required(VERSION 3.23)

project("BSPParser" CXX)
set(CMAKE_CXX_STANDARD 20)

# Traditionally, you shouldn't GLOB source files as it breaks changed file detection.
# However, the "new" CONFIGURE_DEPENDS parameter solves this for most cases with minimal bulid-time overhead
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "*.hpp")

add_library(${PROJECT_NAME})
target_sources(
        ${PROJECT_NAME}
        PRIVATE ${sources}
        PUBLIC FILE_SET HEADERS BASE_DIRS packages FILES ${headers}
)

target_include_directories(${PROJECT_NAME} PRIVATE packages)

include(GNUInstallDirs)

install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        FILE_SET HEADERS
)
install(
        EXPORT ${PROJECT_NAME}
        DESTINATION share/${PROJECT_NAME}
        FILE ${PROJECT_NAME}Config.cmake
        NAMESPACE TAServers::
)
