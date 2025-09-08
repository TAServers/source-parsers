cmake_minimum_required(VERSION 3.28)

include(GNUInstallDirs)

function(RegisterPublicPackage PACKAGE_NAME)
  message(STATUS "Registering public package '${PACKAGE_NAME}'")

  # Traditionally, you shouldn't GLOB source files as it breaks changed file detection.
  # However, the "new" CONFIGURE_DEPENDS parameter solves this for most cases with minimal bulid-time overhead
  file(GLOB_RECURSE sources CONFIGURE_DEPENDS "packages/${PACKAGE_NAME}/*.cpp")
  file(GLOB_RECURSE headers CONFIGURE_DEPENDS "packages/${PACKAGE_NAME}/*.hpp")

  add_library(${PACKAGE_NAME})
  target_sources(
          ${PACKAGE_NAME}
          PRIVATE ${sources}
          PUBLIC FILE_SET HEADERS BASE_DIRS packages FILES ${headers}
  )

  target_link_libraries(${PACKAGE_NAME} PRIVATE SourceParsersShared)
  target_include_directories(${PACKAGE_NAME} PRIVATE packages)

  install(
          TARGETS ${PACKAGE_NAME}
          EXPORT ${PACKAGE_NAME}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          FILE_SET HEADERS
  )
  install(
          DIRECTORY packages/shared
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PACKAGE_NAME}
          FILES_MATCHING PATTERN "*.hpp"
  )
  install(
          EXPORT ${PACKAGE_NAME}
          DESTINATION share/${PACKAGE_NAME}
          FILE ${PACKAGE_NAME}Config.cmake
          NAMESPACE taservers::
  )
endfunction()
