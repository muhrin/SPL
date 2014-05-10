

cmake_minimum_required(VERSION 2.6)

set(SCHEMER_VERSION_MAJOR 0)
set(SCHEMER_VERSION_MINOR 1)
set(SCHEMER_VERSION_PATCH 1)
set(SCHEMER_VERSION "${SCHEMER_VERSION_MAJOR}.${SCHEMER_VERSION_MINOR}.${SCHEMER_VERSION_PATCH}")

message(STATUS "Configuring Schemer ${SCHEMER_VERSION}")

project(Schemer)

## Configuration options ###############################

set(SCHEMER_BUILD_TESTS FALSE CACHE BOOL "Build schemer tests.")

configure_file (
  "${PROJECT_SOURCE_DIR}/include/schemer/SchemerConfig.h.in"
  "${PROJECT_BINARY_DIR}/include/schemer/SchemerConfig.h"
)

## End configuration options ###########################

## FIND PACKAGES ########################################

## Boost ##
# Disable auto-linking
add_definitions(-DBOOST_ALL_NO_LIB)
find_package(Boost 1.36.0 REQUIRED)

# Yaml
find_package(YamlCpp REQUIRED)

## END FIND PACKAGES ####################################

# Include directories with header files
set(SCHEMER_INCLUDE_DIRS
  ${Boost_INCLUDE_DIRS}
  ${YAML_CPP_INCLUDE_DIRS}
  ${PROJECT_BINARY_DIR}/include  # So that we find SchemerConfig.h
  ${PROJECT_SOURCE_DIR}/include
)

include_directories(
  ${SCHEMER_INCLUDE_DIRS}
)

set_property(TARGET PROPERTY PROJECT_LABEL "Schemer")


set(schemer_Header_Files
  include/schemer/Assert.h
  include/schemer/Enumeration.h
  include/schemer/List.h
  include/schemer/Map.h
  include/schemer/Parse.h
  include/schemer/Registry.h
  include/schemer/Scalar.h
  include/schemer/Schemer.h
  include/schemer/Types.h
  include/schemer/Variant.h
  include/schemer/YamlTranscode.h
)
source_group("Header Files" FILES ${schemer_Header_Files})

set(schemer_Header_Files__detail
  include/schemer/detail/Enumeration.h
  include/schemer/detail/List.h
  include/schemer/detail/Map.h
  include/schemer/detail/Type.h
  include/schemer/detail/Variant.h
)
source_group("Header Files" FILES ${schemer_Header_Files})

set(schemer_Source_Files
  src/Parse.cpp
)
source_group("Source Files" FILES ${schemer_Source_Files})

set(schemer_Files
  ${schemer_Header_Files}
  ${schemer_Source_Files}
)

#####################
## Schemer Library ##
#####################
add_library(schemer
  ${schemer_Files}
)

# Libraries we need to link to
target_link_libraries(schemer
  ${Boost_LIBRARIES}
  ${YAML_CPP_LIBRARIES}
)

##########
# Tests ##
##########
if(SCHEMER_BUILD_TESTS)
  add_subdirectory(tests)
endif(SCHEMER_BUILD_TESTS)



