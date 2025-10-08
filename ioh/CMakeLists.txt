# add_subdirectory(${EXTERNAL_DIR}/pybind11 build)

if (CMAKE_VERSION VERSION_LESS 3.18)
  set(DEV_MODULE Development)
else()
  set(DEV_MODULE Development.Module)
endif()
  
find_package(Python 3.8 COMPONENTS Interpreter ${DEV_MODULE} REQUIRED)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

execute_process(
  COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
  OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE nanobind_ROOT)

find_package(nanobind CONFIG REQUIRED)

file(GLOB SOURCES src/*.cpp)
nanobind_add_module(iohcpp ${SOURCES})
    

target_link_libraries(iohcpp PRIVATE ${PROJECT_NAME})  
target_compile_definitions(iohcpp PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
target_precompile_headers(iohcpp PRIVATE src/pch.hpp)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    find_program(MOLD_PROGRAM mold)
    find_program(LDD_PROGRAM ldd)
    find_program(LD_PROGRAM ld)
    if (MOLD_PROGRAM)
        target_link_options(iohcpp PRIVATE -fuse-ld=mold)
        message(STATUS "mold found: ${MOLD_PROGRAM}")
    elseif (LDD_PROGRAM)
        target_link_options(iohcpp PRIVATE -fuse-ld=lld)
        message(STATUS "mold not found using ldd")
    elseif (LD_PROGRAM)
        message(STATUS "both mold and ldd not found using ld")
        target_link_options(iohcpp PRIVATE -Wl,--threads)
    endif ()
    if (PROFILE_BUILD AND NOT MOLD_PROGRAM)
        target_link_options(iohcpp PRIVATE --time-trace)
    endif()
endif()

if (PROFILE_BUILD)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ftime-trace -ftime-report")
    endif()
endif()
