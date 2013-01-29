###
### Creates a .pc file for a library so that pkg-config can
### identify it
###
MACRO(install_pkgconfig_file target pfile)
  # Pkg-config file
  # fill data
  set(PC_LIBS ${${target}_LIBS})

  # lib paths
  get_property(__LFLAGS DIRECTORY PROPERTY LINK_DIRECTORIES)
  FOREACH(FF ${__LFLAGS})
    SET(CONF_LIBS "-L${FF} ${CONF_LIBS}")
  ENDFOREACH(FF)

  # libs
  FOREACH(FF ${PC_LIBS})
    # get the file name, without the directories or .so.
    GET_FILENAME_COMPONENT(FF_WE ${FF} NAME)
    # remove the prefix lib if it exists
    string(REGEX REPLACE "^lib" "" FF_WE ${FF_WE})
    string(REGEX REPLACE "\\.so$" "" FF_WE ${FF_WE})
    SET(CONF_LIBS "${CONF_LIBS} -l${FF_WE}")
  ENDFOREACH(FF)
  # add self
  SET(CONF_LIBS "-L${CMAKE_INSTALL_PREFIX}/lib ${CONF_LIBS} -l${target}")

  # Include dirs.
  get_property(__CFLAGS DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
  FOREACH(FF ${__CFLAGS})
    SET(CONF_CFLAGS "-I${FF} ${CONF_CFLAGS}")
  ENDFOREACH(FF)
  SET(CONF_CFLAGS "-I${CMAKE_INSTALL_PREFIX}/include ${CONF_CFLAGS}")

  # file.in -> file
  string(REGEX REPLACE "\\.in$" "" pfile_out_tmp ${pfile})
  set(pfile_out ${CMAKE_CURRENT_BINARY_DIR}/${pfile_out_tmp})
  configure_file(${pfile} ${pfile_out} @ONLY)

  # Install it
  INSTALL(FILES ${pfile_out} DESTINATION share/pkgconfig )
ENDMACRO(install_pkgconfig_file)

