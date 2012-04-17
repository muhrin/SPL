

function(get_headers search_dir)


  set(found_h FALSE)
  file(RELATIVE_PATH search_dir_rel ${CMAKE_CURRENT_SOURCE_DIR} ${search_dir})

  file(GLOB paths RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${search_dir}/*")

  # Loop over all paths checking for headers and directories
  foreach(path ${paths})

  get_filename_component(pathabs ${path} ABSOLUTE)
  
  #message("Globbing...${path}")

    if(IS_DIRECTORY ${pathabs})
      #message("Found directory...${pathabs}")
      get_headers(${pathabs})
    else(IS_DIRECTORY ${pathabs})

      get_filename_component(ext ${path} EXT)

      if("${ext}" STREQUAL ".h")
        set(group_files
	  ${group_files}
          ${path}
        )
        set(found_h ON BOOL)
      endif("${ext}" STREQUAL ".h")

    endif(IS_DIRECTORY ${pathabs})

  endforeach(path ${paths})

  # Did we find any headers?
  if(found_h)
    message("Found_h: ${found_h}")

    set(result
      ${result}
      ${group_files}
    )

    set(group_name "Header Files\\\\${search_dir_rel}")
    string(REPLACE "/" "\\\\" group_name ${group_name})
    #message("Group name: ${group_name}, files: ${group_files}\n")
    source_group(${group_name} FILES ${group_files})

  endif(found_h)

  # Finally pass on any include files to the outer scope
  set(result
    ${result}
    PARENT_SCOPE
  )

endfunction(get_headers)

set(result)

# Get the headers from this folder onwards
get_headers(${CMAKE_CURRENT_SOURCE_DIR})


message("Headers are: ${result}")

set(sslic_inc ${result} PARENT_SCOPE)
