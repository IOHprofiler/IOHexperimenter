## Build the ioh library.
##
cmake_minimum_required(VERSION 3.15) 
file(READ VERSION VERSION_STRING)
string(STRIP ${VERSION_STRING} ${VERSION_STRING})
string(REGEX REPLACE "\n$" "" VERSION_STRING "${VERSION_STRING}")
project(ioh    
    VERSION "${VERSION_STRING}"
    LANGUAGES CXX  
) 
  
add_compile_definitions(PROJECT_VER="${CMAKE_PROJECT_VERSION}")
set(CMAKE_CXX_STANDARD 17)  
set(EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/external")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH}) 
 
option(BUILD_PYTHON_PACKAGE "Whether to build Python package" OFF)
option(BUILD_TESTS "Whether to build test cases with gtest" ON)
option(BUILD_EXAMPLE "Whether to build Python package" ON)  
option(BUILD_DOCS "Build docs" OFF)    
option(BUILD_SERVER "Whether to build the objective function server" OFF)
option(ENABLE_MKLANDSCAPE_PROBLEMS "Enbale to build mk-landscape problems" OFF) # Turn this option ON if you plan to test MK-landscape problems.

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /bigobj")
endif()   
 
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")  
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++17 -lstdc++fs")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffast-math")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6.0 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        link_libraries(stdc++fs)
        add_compile_definitions(FSEXPERIMENTAL)
    endif()
endif()  



# Define project targets.
file(GLOB_RECURSE HEADERS include/ioh *.hpp) 
   
 
add_library(ioh INTERFACE)
add_library(ioh::ioh ALIAS ioh)
if (MSVC)
set_property(TARGET ioh PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()   

#Include external formatting lib
include_directories(${EXTERNAL_DIR}/fmt/include/)
add_subdirectory(${EXTERNAL_DIR}/fmt)
target_link_libraries(ioh INTERFACE fmt::fmt-header-only)

include_directories(${EXTERNAL_DIR}/clutchlog)

# Add Json lib 
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(${EXTERNAL_DIR}/json)
target_link_libraries(ioh INTERFACE nlohmann_json::nlohmann_json)


if(ENABLE_MKLANDSCAPE_PROBLEMS)

    # Adding Macro for C++ Headers
    add_compile_definitions(USING_MKLANDSCAPE)

    # Find cargo executable
    find_program(CARGO_EXECUTABLE cargo)
    message(${CARGO_EXECUTABLE})
    if(${CARGO_EXECUTABLE} STREQUAL CARGO_EXECUTABLE-NOTFOUND)
        execute_process(
            COMMAND bash -c "curl https://sh.rustup.rs -sSf | sh -s -- -y"
            OUTPUT_VARIABLE outVar)
        execute_process(
            COMMAND bash -c "source $HOME/.cargo/env"
            OUTPUT_VARIABLE outVar)
    endif()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${EXTERNAL_DIR}/MkLandscape/release)
    
    add_subdirectory(${EXTERNAL_DIR}/MkLandscape ${PROJECT_SOURCE_DIR}/include/ioh/problem/mklandscape/)
    target_link_libraries(ioh INTERFACE problem_generator_lib)
    
    add_compile_definitions(MK_COMPILED)
endif()


target_include_directories(ioh
INTERFACE
    # Interface paths are relative to the installation path, not the current directory.
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include> 
)

target_compile_features(ioh
INTERFACE
    cxx_std_17
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

install(TARGETS ioh fmt-header-only nlohmann_json
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
if(BUILD_TESTS) 
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

if(BUILD_DOCS)
    add_subdirectory(doc)
endif()

if(BUILD_PYTHON_PACKAGE)
    add_subdirectory(ioh)
endif()

if(BUILD_SERVER)
    include_directories(${EXTERNAL_DIR}/cxxopts/include/)
    add_subdirectory(server)
endif()

