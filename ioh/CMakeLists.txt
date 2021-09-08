add_subdirectory(${EXTERNAL_DIR}/pybind11 build)

file(GLOB SOURCES src/*.cpp)
pybind11_add_module(iohcpp ${SOURCES})

target_link_libraries(iohcpp PRIVATE ${PROJECT_NAME})  
target_compile_definitions(iohcpp PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})