#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ioh::ioh" for configuration "Debug"
set_property(TARGET ioh::ioh APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(ioh::ioh PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/ioh.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS ioh::ioh )
list(APPEND _IMPORT_CHECK_FILES_FOR_ioh::ioh "${_IMPORT_PREFIX}/lib/ioh.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
