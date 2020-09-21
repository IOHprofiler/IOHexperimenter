## Build the ioh library.
##
cmake_minimum_required(VERSION 3.11)

project(ioh
    VERSION 0.0.0.1
    LANGUAGES CXX
)
 
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif() 


# Define project targets.
file(GLOB SOURCES src/*.cpp src/*/*.cpp)
file(GLOB_RECURSE HEADERS include/ioh *.hpp) 

add_library(ioh ${HEADERS} ${SOURCES})
add_library(ioh::ioh ALIAS ioh)

target_include_directories(ioh
PUBLIC
    # Interface paths are relative to the installation path, not the current
    # directory.
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(ioh
PUBLIC
    cxx_std_17
)

target_compile_options(ioh
PRIVATE
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

install(
TARGETS 
    ioh
EXPORT ioh-targets
LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
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
add_subdirectory("example")
