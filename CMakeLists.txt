cmake_minimum_required(VERSION 3.28)
project(asyncio-cpp VERSION 0.1.0 LANGUAGES CXX)

add_library(asyncio)
target_compile_features(asyncio PUBLIC cxx_std_23)
set_target_properties(asyncio PROPERTIES CXX_EXTENSIONS OFF)
target_sources(asyncio
  PUBLIC FILE_SET CXX_MODULES
  BASE_DIRS src
  FILES
    src/asyncio.cppm
    src/task.cppm
    src/event_loop.cppm
)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(TARGETS asyncio
  EXPORT AsyncioTargets
  FILE_SET CXX_MODULES
  DESTINATION share/asyncio/modules
)

install(EXPORT AsyncioTargets
  FILE AsyncioTargets.cmake
  NAMESPACE asyncio::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/asyncio
)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/AsyncioConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/AsyncioConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/asyncio
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/asyncioConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/AsyncioConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/asyncioConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/asyncio
)
