add_subdirectory(${EXTERNAL_DIR}/pybind11 build)


file(GLOB SOURCES src/*.cpp)
pybind11_add_module(iohcpp ${SOURCES})

target_link_libraries(iohcpp PRIVATE ${PROJECT_NAME})  
target_compile_definitions(iohcpp PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
target_precompile_headers(iohcpp PRIVATE src/pch.hpp)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    find_program(MOLD_PROGRAM mold)
    find_program(LDD_PROGRAM ldd)
    # if (MOLD_PROGRAM)
    #     target_link_options(iohcpp PRIVATE -fuse-ld=mold)
    #     message(STATUS "mold found: ${MOLD_PROGRAM}")
        # target_link_options(iohcpp PRIVATE
        # -Wl,--perf                 # mold: print per-phase timings
        # -Wl,--stats                # mold: print input/object stats
        # -Wl,-Map=${CMAKE_BINARY_DIR}/link.map  # classic map for size/symbol audit
        # )
        # set_property(TARGET iohcpp PROPERTY INTERPROCEDURAL_OPTIMIZATION FALSE)
        # target_compile_options(iohcpp PRIVATE -fno-lto)
        # target_link_options(iohcpp PRIVATE -fno-lto)
    if (LDD_PROGRAM)
        target_link_options(iohcpp PRIVATE -fuse-ld=lld)
        message(STATUS "mold not found using ldd")
    else()
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
