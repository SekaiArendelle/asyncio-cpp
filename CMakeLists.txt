cmake_minimum_required(VERSION 3.28)
project(asyncio-cpp VERSION 0.0.1 LANGUAGES CXX)

set(ASYNCIO_SANITIZER "" CACHE STRING "Enable sanitizers: address, undefined (comma-separated)")

if(ASYNCIO_SANITIZER)
    string(REPLACE "," ";" _san_list "${ASYNCIO_SANITIZER}")
    foreach(san ${_san_list})
        string(STRIP "${san}" san)
        if(san STREQUAL "address")
            message(STATUS "[asan] AddressSanitizer enabled")
            list(APPEND _san_cflags -fsanitize=address -fno-omit-frame-pointer)
            list(APPEND _san_lflags -fsanitize=address)
        elseif(san STREQUAL "undefined")
            message(STATUS "[ubsan] UndefinedBehaviorSanitizer enabled")
            list(APPEND _san_cflags -fsanitize=undefined)
            list(APPEND _san_lflags -fsanitize=undefined)
        else()
            message(WARNING "Unknown sanitizer: ${san}")
        endif()
    endforeach()
    add_compile_options(${_san_cflags})
    add_link_options(${_san_lflags})
endif()

add_library(asyncio)
target_compile_features(asyncio PUBLIC cxx_std_23)
set_target_properties(asyncio PROPERTIES CXX_EXTENSIONS OFF)
target_sources(asyncio
  PUBLIC FILE_SET CXX_MODULES
  BASE_DIRS src
  FILES
    src/lib.cppm
    src/task.cppm
    src/details/chrono.cppm
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
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/asyncioConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/asyncioConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/asyncio
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/asyncioConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/asyncioConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/asyncioConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/asyncio
)
