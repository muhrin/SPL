cmake_minimum_required(VERSION 2.6)

project(SPL)

set(SPL_VERSION_MAJOR 0)
set(SPL_VERSION_MINOR 4)
set(SPL_VERSION_PATCH 1)
set(SPL_VERSION "${SPL_VERSION_MAJOR}.${SPL_VERSION_MINOR}.${SPL_VERSION_PATCH}")

message(STATUS "Configuring SPL ${SPL_VERSION}")

include(cmake/utility.cmake)

## FIND PACKAGES ########################################

# Boost #
# Disable auto-linking
add_definitions(-DBOOST_ALL_NO_LIB)
# Check if we can find boost log first
set(SPL_BOOST_COMPONENTS system filesystem regex)
find_package(Boost COMPONENTS log QUIET)
if(Boost_LOG_FOUND)
  set(SPL_BOOST_COMPONENTS ${SPL_BOOST_COMPONENTS} log)
endif(Boost_LOG_FOUND)
if(SSLIB_ENABLE_THREAD_AWARE)
  set(SPL_BOOST_COMPONENTS ${SPL_BOOST_COMPONENTS} thread)
endif(SSLIB_ENABLE_THREAD_AWARE)

# Finally try to find boost and all the components we need
find_package(Boost 1.36.0 REQUIRED COMPONENTS ${SPL_BOOST_COMPONENTS} QUIET)

#
# Armadillo #
find_package(Armadillo REQUIRED)
if("${ARMADILLO_INCLUDE_DIRS}" STREQUAL "")
  set(ARMADILLO_INCLUDE_DIRS "${ARMADILLO_ROOT}/include")
endif()

#
# CGAL #
find_package(CGAL QUIET COMPONENTS Core QUIET)
if(CGAL_FOUND)
  include( ${CGAL_USE_FILE} )
endif(CGAL_FOUND)

find_package(Spglib REQUIRED QUIET)
find_package(YamlCpp QUIET)
find_package_or_install(Schemer)

## END FIND PACKAGES ####################################

## CONFIGURATION SETTINGS ##############################

# Build options ###
set(SPL_BUILD_TESTS FALSE CACHE BOOL "Build spl tests")
set(SSLIB_ENABLE_THREAD_AWARE TRUE CACHE BOOL "Enable awareness of multithreaded environment (requires Boost thread)")

set(SPL_USE_CGAL ${CGAL_FOUND} CACHE INTERNAL "Build SPL with CGAL support.")
set(SPL_USE_YAML ${YamlCpp_FOUND} CACHE INTERNAL "Build SPL with Yaml support.")
set(SPL_USE_BOOST_LOG ${Boost_LOG_FOUND} CACHE INTERNAL "Use boost log for logging.")

configure_file(
  "${PROJECT_SOURCE_DIR}/include/spl/config.h.in"
  "${PROJECT_BINARY_DIR}/include/spl/config.h"
)
configure_file(
  "${PROJECT_SOURCE_DIR}/include/spl/version.h.in"
  "${PROJECT_BINARY_DIR}/include/spl/version.h"
)

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_INCLUDE_DIR include CACHE PATH  "Installation directory for header files"
)
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR share/SPL/cmake)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for
 CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

## END CONFIGURATION SETTINGS ##########################

# Process header and source files
include(Include.cmake)
include(Src.cmake)

## INCLUDE DIRECTORIES ##################################

set(SSLIB_LOCAL_INCLUDE_DIRS
  ${PROJECT_BINARY_DIR}/include  # So that we find cmake configured headers
  ${PROJECT_SOURCE_DIR}/include
  CACHE INTERNAL "Internal variable used to set the local include directories used by SPL"
  FORCE
)

# Include directories with header files, have to use BEFORE
# so that these get used before the system and local include dirs
# otherwise the project may not use the library locations we want
include_directories(BEFORE
  ${Boost_INCLUDE_DIRS}
  ${ARMADILLO_INCLUDE_DIRS}
  ${SPGLIB_INCLUDE_DIRS}
  ${SCHEMER_INCLUDE_DIRS}
  ${SSLIB_LOCAL_INCLUDE_DIRS}
)

# Include YAML if we're using it
if(SPL_USE_YAML)
  include_directories(${YAML_CPP_INCLUDE_DIRS})
endif(SPL_USE_YAML)

# Include CGAL if we're using it
if(SPL_USE_CGAL)
  include_directories("${CGAL_INCLUDE_DIRS}" )
endif(SPL_USE_CGAL)

set_property(TARGET PROPERTY PROJECT_LABEL "SPL")

## BUILD SSLIB #############################################

add_library(spl
  ${sslib_Header_Files}
  ${sslib_Source_Files}
)

# Libraries we need to link to
target_link_libraries(spl
  ${Boost_LIBRARIES}
  ${SPGLIB_LIBRARIES}
  ${ARMADILLO_LIBRARIES}
  ${SCHEMER_LIBRARIES}
)

if(SPL_USE_YAML)
  target_link_libraries(spl
    ${YAML_CPP_LIBRARIES}
    schemer
  )
endif(SPL_USE_YAML)

if(SPL_USE_CGAL)
  target_link_libraries(spl
    ${CGAL_LIBRARY}
    ${CGAL_Core_LIBRARY}
  )
endif(SPL_USE_CGAL)

install(TARGETS spl
  EXPORT SPLTargets
  PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDE_DIR} COMPONENT dev
  LIBRARY DESTINATION ${INSTALL_LIB_DIR} COMPONENT shlib
  ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
)
install(DIRECTORY include/ DESTINATION ${INSTALL_INCLUDE_DIR}
  COMPONENT dev
  PATTERN "*.in" EXCLUDE
)
install(DIRECTORY ${PROJECT_BINARY_DIR}/include/ DESTINATION ${INSTALL_INCLUDE_DIR}
  COMPONENT dev
)


###
### Configuration file settings
###
# Add all targets to the build-tree export set
export(TARGETS spl FILE "${PROJECT_BINARY_DIR}/SPLTargets.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE SPL)

# Create the SpglibConfig.cmake and SpglibConfigVersion.cmake files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS
  "${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}/include"
)
configure_file(cmake/SPLConfig.cmake.in
  "${PROJECT_BINARY_DIR}/SPLConfig.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${SPL_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(cmake/SPLConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/SPLConfig.cmake" @ONLY)

# ... for both
configure_file(cmake/SPLConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/SPLConfigVersion.cmake" @ONLY)

# Install the *Config.cmake and *ConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/SPLConfig.cmake"
  "${PROJECT_BINARY_DIR}/SPLConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT SPLTargets DESTINATION
  "${INSTALL_CMAKE_DIR}" COMPONENT dev)


###########
## Tests ##
###########

if(SPL_BUILD_TESTS)
  add_subdirectory(tests)
endif(SPL_BUILD_TESTS)

