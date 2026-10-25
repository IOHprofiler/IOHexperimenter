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

# The ioh library holds problem code, so build it optimized unless a build type is given (single-configuration
# generators; project() already created the empty cache entry)
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR AND NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)  
set(EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/external")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH}) 
  
option(BUILD_PYTHON_PACKAGE "Whether to build Python package" OFF)
option(BUILD_TESTS "Whether to build test cases with gtest" ON)
option(BUILD_EXAMPLE "Whether to build examples" ON)  
option(BUILD_DOCS "Build docs" OFF)    
option(BUILD_SERVER "Whether to build the objective function server" OFF) 
option(ENABLE_MKLANDSCAPE_PROBLEMS "Enbale to build mk-landscape problems" OFF) # Turn this option ON if you plan to test MK-landscape problems.

# Link-time optimization of the release configurations. Defining CMAKE_INTERPROCEDURAL_OPTIMIZATION also keeps
# pybind11 from enabling its own (full) LTO for the Python extension. On by default only for the Python extension
# with MSVC, which compiles /GL objects without generating code (without it the Windows wheels built slower than
# before the compiled registry). Elsewhere it does not change the evaluation speed (measured with clang), and a
# static library of LTO objects only links with the toolchain that built it.
if(MSVC AND BUILD_PYTHON_PACKAGE)
    set(IOH_LTO_DEFAULT ON)
else()
    set(IOH_LTO_DEFAULT OFF)
endif()
option(IOH_LTO "Build the release configurations with link-time optimization" ${IOH_LTO_DEFAULT})
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)
foreach(config RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_${config} ${IOH_LTO})
endforeach()
  
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /bigobj")
endif()    

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")   
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion -Wall -Wextra -pedantic -std=c++17 -lstdc++fs")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 ")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 6.0 AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        link_libraries(stdc++fs) 
        add_compile_definitions(FSEXPERIMENTAL)
    endif() 
endif()

# find_package(OpenMP)
# if (OPENMP_FOUND)
#     set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#     set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#     set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
# endif() 
 
# Define project targets.
file(GLOB_RECURSE HEADERS include/ioh *.hpp) 
   
 
# The compiled part of the library: where the data files are (src/data.cpp), and the registries of the built-in
# problems and suites (src/registry), so that only these files instantiate every problem class
add_library(ioh STATIC
    src/data.cpp
    src/registry/bbob.cpp
    src/registry/cec.cpp
    src/registry/dynamic_bin_val.cpp
    src/registry/graph.cpp
    src/registry/pbo.cpp
    src/registry/single_objective.cpp
    src/registry/star_discrepancy.cpp
    src/registry/suites.cpp
)
add_library(ioh::ioh ALIAS ioh)
# position-independent: it is also linked into the Python extension. The Debug build gets its own file name, so
# that installed Debug and Release builds can coexist.
set(IOH_DEBUG_POSTFIX d CACHE STRING "Postfix of the Debug build of the ioh library")
set_target_properties(ioh PROPERTIES POSITION_INDEPENDENT_CODE ON DEBUG_POSTFIX "${IOH_DEBUG_POSTFIX}")
# MSBuild (the Visual Studio generator, which setup.py uses on Windows) compiles a project's files one after
# another unless /MP is given; pybind11 sets it on the extension module only
if(CMAKE_GENERATOR MATCHES "Visual Studio" AND CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(ioh PRIVATE /MP)
endif()

#Include external formatting lib
include_directories(${EXTERNAL_DIR}/fmt/include/)
add_subdirectory(${EXTERNAL_DIR}/fmt)
target_link_libraries(ioh PUBLIC fmt::fmt-header-only)

# Add Json lib 
if(BUILD_SERVER AND EXISTS ${EXTERNAL_DIR}/json/CMakeLists.txt)
    set(INCLUDE_JSON true)
    add_compile_definitions(HAS_JSON)
    set(JSON_Install OFF CACHE INTERNAL "")
    add_subdirectory(${EXTERNAL_DIR}/json)
    target_link_libraries(ioh PUBLIC nlohmann_json::nlohmann_json)
endif()

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
    target_link_libraries(ioh PUBLIC problem_generator_lib)
    
    add_compile_definitions(MK_COMPILED)
endif()


target_include_directories(ioh
PUBLIC
    # Interface paths are relative to the installation path, not the current directory.
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include> 
)

target_compile_features(ioh
PUBLIC
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

# The data files, and where the library finds them by default (see include/ioh/common/data.hpp): the static folder
# of this source tree while it exists, then the installed copy. The Python package sets its own.
install(DIRECTORY static/ DESTINATION ${CMAKE_INSTALL_DATADIR}/ioh)
if(NOT BUILD_PYTHON_PACKAGE)
    target_compile_definitions(ioh PRIVATE
        IOH_SOURCE_DATA_DIR="${PROJECT_SOURCE_DIR}/static"
        IOH_INSTALL_DATA_DIR="${CMAKE_INSTALL_FULL_DATADIR}/ioh")
endif()


if (DEFINED INCLUDE_JSON)
install(TARGETS ioh fmt-header-only nlohmann_json
    EXPORT ioh-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
) 
else()
install(TARGETS ioh fmt-header-only 
    EXPORT ioh-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
) 
endif()

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

