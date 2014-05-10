

function(try_installing lib ret_install_path)

  # Check to see if the file external_<lib>.cmake exists in which
  # case it will be used as the CMakeLists.txt to install that
  # library
  set(CMAKE_FILE "${CMAKE_SOURCE_DIR}/cmake/external_${lib}.cmake")
  if(EXISTS "${CMAKE_FILE}")
    # Create the build dir
    set(BUILD_DIR "${CMAKE_BINARY_DIR}/${lib}")
    file(MAKE_DIRECTORY ${BUILD_DIR})

    # Copy over the file that will become CMakeLists.txt
    file(COPY ${CMAKE_FILE} DESTINATION ${BUILD_DIR})
    file(RENAME "${BUILD_DIR}/external_${lib}.cmake"
      "${BUILD_DIR}/CMakeLists.txt")
    
    # Use the same install prefix as us
    if(NOT CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
      set(INSTALL_PREFIX "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")
    endif()

    message(STATUS "Installing ${lib}...")

    # Configure and build
    execute_process(COMMAND ${CMAKE_COMMAND} 
      "${INSTALL_PREFIX}" "." 
      WORKING_DIRECTORY ${BUILD_DIR} OUTPUT_QUIET)
    execute_process(COMMAND ${CMAKE_COMMAND} "--build" "." "--target"
      WORKING_DIRECTORY ${BUILD_DIR})

    # Tell our caller where we installed the lib to
    set(${ret_install_path}
      "${BUILD_DIR}/install" PARENT_SCOPE
    )
  endif(EXISTS "${CMAKE_FILE}")

endfunction(try_installing)

# Save to use a macro as opposed to function because we're calling
# find_package within our scope
# see: http://cmake.3232098.n2.nabble.com/Scope-of-find-package-inside-a-function-block-td6821980.html
macro(find_package_or_install lib)

  find_package(${lib} QUIET)
  if(NOT ${${lib}_FOUND})
    try_installing(${lib} INSTALL_PATH)
    # Did the install succeed?
    find_package(${lib} QUIET REQUIRED HINTS ${INSTALL_PATH})
    if(INSTALL_PATH)
      # Add the build path of the library as a subdirectory so that
      # it is rebuilt if it changes
      add_subdirectory("${CMAKE_BINARY_DIR}/${lib}" "${CMAKE_BINARY_DIR}")
      unset(INSTALL_PATH)
    endif(INSTALL_PATH)
  endif()
  if(${${lib}_FOUND})
    message(STATUS "Found ${lib}: ${${lib}_DIR} (v${${lib}_VERSION})")
  endif()

endmacro(find_package_or_install)
