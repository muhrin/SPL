
# This is the first version with git support for external projects
cmake_minimum_required(VERSION 2.8.2)

include(ExternalProject)

set(lib Schemer)

if(NOT CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(${lib}_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}")
else()
  set(${lib}_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/install")
endif()

set(${lib}_CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${${lib}_INSTALL_DIR})

ExternalProject_Add(${lib}
  GIT_REPOSITORY git@github.com:muhrin/schemer.git
  GIT_TAG master
  INSTALL_DIR ${${lib}_INSTALL_DIR}
  CMAKE_ARGS ${${lib}_CMAKE_ARGS}
)

export(PACKAGE ${lib})
