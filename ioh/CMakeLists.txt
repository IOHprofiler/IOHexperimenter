add_subdirectory(${EXTERNAL_DIR}/pybind11 build)


file(GLOB SOURCES src/*.cpp)
pybind11_add_module(iohcpp ${SOURCES})

target_link_libraries(iohcpp PRIVATE ${PROJECT_NAME})  
target_compile_definitions(iohcpp PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
target_precompile_headers(iohcpp PRIVATE src/pch.hpp)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    find_program(MOLD_PROGRAM mold)
    find_program(LDD_PROGRAM ldd)
    if (MOLD_PROGRAM)
        add_link_options(-fuse-ld=mold)
        message(STATUS "mold found: ${MOLD_PROGRAM}")
    elseif (LDD_PROGRAM)
        add_link_options(-fuse-ld=lld) 
        message(STATUS "mold not found using ldd")
    else()
        message(STATUS "both mold and ldd not found using ld")
        add_link_options(-Wl,--threads)
    endif ()
endif()

# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ftime-trace -ftime-report")

