
## Configure the tests header file

## CONFIGURATION SETTINGS ##############################

# Build options ###
if(Boost_USE_STATIC_LIBS)
else()
  add_definitions(-DBOOST_TEST_DYN_LINK)
endif(Boost_USE_STATIC_LIBS)

## END CONFIGURATION SETTINGS ##########################

find_package(Boost 1.36.0 REQUIRED COMPONENTS system filesystem unit_test_framework regex)

# tests/analysis

set(tests_Source_Files__analysis
  analysis/VoronoiMapTest.cpp
)
source_group("Source Files\\analysis" FILES ${tests_Source_Files__analysis})

# tests/build_cell

set(tests_Source_Files__build_cell
  build_cell/AtomExtruderTest.cpp
)
source_group("Source Files\\build_cell" FILES ${tests_Source_Files__build_cell})

# tests/common

set(tests_Source_Files__common
  common/DistanceCalculatorsTest.cpp
  common/UnitCellTest.cpp
)
source_group("Source Files\\common" FILES ${tests_Source_Files__common})

# tests/factory

set(tests_Source_Files__factory
  factory/YamlFactoryTest.cpp
)
source_group("Source Files\\factory" FILES ${tests_Source_Files__factory})
set(tests_Input_Files__factory
  factory/RandomStructure.sslib
)

## tests/utility

set(tests_Header_Files__utility
)
source_group("Header Files\\utility" FILES ${tests_Header_Files__utility})

set(tests_Source_Files__utility
  utility/HeterogeneousMapTest.cpp
  utility/MultiIdxTest.cpp
  utility/MultiIdxRangeTest.cpp
  utility/PermutationRangeTest.cpp
  utility/StructureComparatorsTest.cpp
)
source_group("Source Files\\utility" FILES ${tests_Source_Files__utility})
set(tests_Input_Files__utility
  utility/similarStructures
)

## tests/

set(tests_Header_Files__
  sslibtest.h
)
source_group("Header Files" FILES ${tests_Header_Files__})

set(tests_Source_Files__
)
source_group("Source Files" FILES ${tests_Source_Files__})

set(tests_Header_Files
  ${tests_Header_Files__utility}
  ${tests_Header_Files__}
)

set(tests_Source_Files
  ${tests_Source_Files__analysis}
  ${tests_Source_Files__build_cell}
  ${tests_Source_Files__common}
  ${tests_Source_Files__factory}
  ${tests_Source_Files__utility}
  ${tests_Source_Files__}
)

set(tests_Files
  ${tests_Header_Files}
  ${tests_Source_Files}
)


## Copy over files needed by tests
file(COPY
  ${tests_Input_Files__factory}
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
)
file(COPY
  ${tests_Input_Files__utility}
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
)

message(STATUS "Copied ${tests_Input_Files__factory} to ${CMAKE_CURRENT_BINARY_DIR}")


#########################
## Include directories ##
#########################

include_directories(
  ${SSLIB_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/tests
)


############################
## SSLibTest executable   ##
############################
add_executable(sslibtest
  ${tests_Files}
  sslibtest.cpp
)

add_dependencies(sslibtest sslib)

# Libraries we need to link to
target_link_libraries(sslibtest
  ${Boost_LIBRARIES}
  ${ARMADILLO_LIBRARIES}
  spglib
  sslib
)
