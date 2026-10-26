add_subdirectory(${EXTERNAL_DIR}/pybind11 build)

file(GLOB SOURCES src/*.cpp)
pybind11_add_module(iohcpp ${SOURCES})

target_link_libraries(iohcpp PRIVATE ${PROJECT_NAME})  
target_compile_definitions(iohcpp PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})

# The Python package (pyproject.toml installs only this component): the extension and the data files
install(TARGETS iohcpp LIBRARY DESTINATION ioh COMPONENT python)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/static/ DESTINATION ioh/static COMPONENT python)
