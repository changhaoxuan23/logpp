cmake_minimum_required(VERSION 3.29)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(LogPP
  VERSION 0.0.1
  DESCRIPTION "easy to use and powerful logger"
  LANGUAGES CXX
)

add_library(LogPP SHARED logpp.cc)
target_compile_options(LogPP PUBLIC -stdlib=libc++)
target_link_options(LogPP PUBLIC -stdlib=libc++)
set_property(TARGET LogPP PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET LogPP PROPERTY SOVERSION 1)
set_property(TARGET LogPP PROPERTY INTERFACE_LogPP_VERSION 1)
set_property(TARGET LogPP APPEND PROPERTY
  COMPATIBLE_INTERFACE_STRING INTERFACE_LogPP_VERSION
)
target_include_directories(LogPP INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)

install(TARGETS LogPP EXPORT LogPPTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)
install(
  FILES
    logpp.hh
  DESTINATION
    include
  COMPONENT
    Devel
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/LogPP/LogPPConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT LogPPTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/LogPP/LogPPTargets.cmake"
)
configure_file(cmake/LogPPConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/LogPP/LogPPConfig.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cmake/LogPP)
install(EXPORT LogPPTargets
  FILE
  LogPPTargets.cmake
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    cmake/LogPPConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/LogPP/LogPPConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)