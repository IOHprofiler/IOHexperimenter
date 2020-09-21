get_filename_component(IOH_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

if(NOT TARGET ioh::ioh)
    include("${IOH_CMAKE_DIR}/ioh-targets.cmake")
endif()

set(IOH_LIBRARIES ioh::ioh)
