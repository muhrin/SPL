
## Configure the tests header file

## CONFIGURATION SETTINGS ##############################

# Build options ###
if(Boost_USE_STATIC_LIBS)
else()
  add_definitions(-DBOOST_TEST_DYN_LINK)
endif(Boost_USE_STATIC_LIBS)

## END CONFIGURATION SETTINGS ##########################

find_package(Boost 1.36.0 REQUIRED COMPONENTS system filesystem unit_test_framework regex)

set(test_folders
  analysis
  build_cell
  common
  factory
  io
  potential
  utility
)

foreach(test_folder ${test_folders})
  # Look for cpp files
  file(GLOB
    tests_Source_Files__${test_folder}
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${test_folder}/*.cpp
  )
  source_group("Source Files\\${test_folder}" FILES ${tests_Source_Files__${test_folder}})
  set(tests_Source_Files
    ${tests_Source_Files}
	${tests_Source_Files__${test_folder}}
  )
  
  # Look for input files
  file(GLOB
    tests_Input_Files__${test_folder}
	RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${test_folder}/input/*
  )
  set(tests_Input_Files
    ${tests_Input_Files}
	${tests_Input_Files__${test_folder}}
  )
endforeach()

## Look for any shared input files
file(GLOB
  tests_Input_Files__shared
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  input/*
)
set(tests_Input_Files
  ${tests_Input_Files}
  ${tests_Input_Files__shared}
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
  ${tests_Header_Files__}
)

set(tests_Files
  ${tests_Header_Files}
  ${tests_Source_Files}
)


## Copy over files needed by tests
message(STATUS "Copying test input files...")
file(COPY
  ${tests_Input_Files}
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
)


#########################
## Include directories ##
#########################

include_directories(
  ${SSLIB_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/tests
)


##########################
## SplTest executable   ##
##########################
add_executable(spltest
  ${tests_Files}
  sslibtest.cpp
)

add_dependencies(spltest sslib)

# Libraries we need to link to
target_link_libraries(spltest
  ${Boost_LIBRARIES}
  ${ARMADILLO_LIBRARIES}
  spglib
  sslib
)
