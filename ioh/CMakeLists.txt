# nanobind comes from the build environment (pyproject.toml). FindPython finds the Python that scikit-build-core
# builds for (Development.Module needs CMake 3.18).
find_package(Python 3.10 REQUIRED COMPONENTS Interpreter Development.Module)
execute_process(
    COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE nanobind_ROOT)
find_package(nanobind CONFIG REQUIRED)

file(GLOB SOURCES src/*.cpp)
# NOMINSIZE: -O3 like the library (nanobind's default -Os makes the evaluation slower)
nanobind_add_module(iohcpp NOMINSIZE ${SOURCES})
target_link_libraries(iohcpp PRIVATE ${PROJECT_NAME})
# the problem families of the registry (src/registry/families.hpp)
target_include_directories(iohcpp PRIVATE ${PROJECT_SOURCE_DIR}/src)

# The Python package (pyproject.toml installs only this component): the extension and the data files
install(TARGETS iohcpp LIBRARY DESTINATION ioh COMPONENT python)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/static/ DESTINATION ioh/static COMPONENT python)
