## Build the ioh library.
##
cmake_minimum_required(VERSION 3.10)

project(ioh
    VERSION 0.0.0.1
    LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

option(ENABLE_PYTHON_PROBLEMS "Enable the ability to call external Python modules as if they were an IOH problem" OFF)
option(BUILD_DOCS "Build docs" OFF)

if(MSVC)
    set(ENABLE_PYTHON_PROBLEMS FALSE) # The call-problem-from-python interface is broken on windows
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif() 

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lstdc++fs")

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6.0 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        link_libraries(stdc++fs)
        add_compile_definitions(FSEXPERIMENTAL)
    endif()
endif()

# Define project targets.
file(GLOB_RECURSE HEADERS include/ioh *.hpp) 
  
# Exlude python problems if not enabled
if(NOT ENABLE_PYTHON_PROBLEMS)
    list(FILTER HEADERS EXCLUDE REGEX "include/ioh/problem/python/*")
endif()


add_library(ioh INTERFACE)
add_library(ioh::ioh ALIAS ioh)


# Add Python Library
if(ENABLE_PYTHON_PROBLEMS)
    find_package(PythonLibs REQUIRED)
    include_directories(${PYTHON_INCLUDE_DIRS})
    target_link_libraries(ioh INTERFACE ${PYTHON_LIBRARIES})
    add_compile_definitions(PYTHON_DEFINED)
endif()


target_include_directories(ioh
INTERFACE
    # Interface paths are relative to the installation path, not the current
    # directory.
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(ioh
INTERFACE
    cxx_std_17
)

target_compile_options(ioh
INTERFACE
    -Wall
    $<$<CXX_COMPILER_ID:GNU>:-pedantic>
)


# Generate package config files.
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/cmake/ioh-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ioh-config.cmake
INSTALL_DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/ioh
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ioh-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)


# Define installation rules.
include(GNUInstallDirs)

install(TARGETS ioh
    EXPORT ioh-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ioh-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ioh-config-version.cmake
DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/ioh
)

install(EXPORT ioh-targets
    FILE ioh-targets.cmake
    NAMESPACE ioh::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ioh
)


# Export targets. If an external project adds this project to its build tree
# via `add_subdirectory()`, then `find_package()` will work without
# requiring this project to be installed.
export(EXPORT ioh-targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/ioh-targets.cmake
    NAMESPACE ioh::
)
export(PACKAGE ioh)


# Add subdirectories to build.
add_subdirectory(test)
add_subdirectory(example)

if(BUILD_DOCS)
    add_subdirectory(doc)
endif()





