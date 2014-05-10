
## Configure the tests header file

## CONFIGURATION SETTINGS ##############################

# Build options ###
if(Boost_USE_STATIC_LIBS)
else()
  add_definitions(-DBOOST_TEST_DYN_LINK)
endif(Boost_USE_STATIC_LIBS)

## END CONFIGURATION SETTINGS ##########################

find_package(Boost 1.36.0 REQUIRED COMPONENTS unit_test_framework)


# Look for cpp files
  file(GLOB
    tests_Source_Files
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
  )
  source_group("Source Files" FILES ${tests_Source_Files})
  
  # Look for input files
  file(GLOB
    tests_Input_Files
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    input/*
  )

set(tests_Header_Files
  schemer_test.h
)
source_group("Header Files" FILES ${tests_Header_Files})

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
  ${SCHEMER_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/tests
)


################################
## Schemer tests executable   ##
################################
add_executable(schemer_test
  ${tests_Files}
)

add_dependencies(schemer_test schemer)

# Libraries we need to link to
target_link_libraries(schemer_test
  ${Boost_LIBRARIES}
  schemer
)
