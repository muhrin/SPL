cmake_minimum_required(VERSION 2.6)

# Paths to libraries - change these! #########
set(BOOST_ROOT "/homes/muhrin/src/boost_1_48_0" CACHE FILEPATH "")
set(ARMADILLO_ROOT "/homes/muhrin/src/armadillo-2.4.2" CACHE FILEPATH "")

project(SSLib)

# Tell CMake where to find modules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeModules/;${CMAKE_MODULE_PATH}")

set(SSLIB_VERSION_MAJOR 0)
set(SSLIB_VERSION_MAJOR 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/SSLibConfig.h.in"
  "${PROJECT_BINARY_DIR}/src/SSLibConfig.h"
  )



## FIND PACKAGES ########################################
# First add any user specified paths to the package search array
set(CMAKE_PREFIX_PATH "${BOOST_ROOT}" "${ARMADILLO_ROOT}")

# Boost #
find_package(Boost 1.36.0 COMPONENTS system filesystem REQUIRED)
# Armadillo #
find_package(Armadillo)
if("${ARMADILLO_INCLUDE_DIRS}" STREQUAL "")
  set(ARMADILLO_INCLUDE_DIRS "${ARMADILLO_ROOT}/include")
endif()

## END FIND PACKAGES ####################################

# Include directories with header files
  include_directories(
    ${Boost_INCLUDE_DIRS}
    ${ARMADILLO_INCLUDE_DIRS}
  )

include_directories(include src)

add_subdirectory(src lib)

set_property(TARGET PROPERTY PROJECT_LABEL "SSLib")
