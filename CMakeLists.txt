cmake_minimum_required(VERSION 2.6)


project(SSLib)

set(SSLIB_VERSION_MAJOR 0)
set(SSLIB_VERSION_MAJOR 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/SSLib.h.in"
  "${PROJECT_BINARY_DIR}/src/SSLib.h"
  )



include_directories(include src)

add_subdirectory(src lib)