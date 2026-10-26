# nanobind comes from the build environment (pyproject.toml). FindPython finds the Python that scikit-build-core
# builds for (Development.Module needs CMake 3.18). For an abi3 wheel (wheel.py-api in pyproject.toml, Python 3.12
# and later) scikit-build-core sets SKBUILD_SABI_COMPONENT to Development.SABIModule (CMake 3.26).
find_package(Python 3.10 REQUIRED COMPONENTS Interpreter Development.Module ${SKBUILD_SABI_COMPONENT})
execute_process(
    COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE nanobind_ROOT)
find_package(nanobind CONFIG REQUIRED)

file(GLOB SOURCES src/*.cpp)
# NOMINSIZE: -O3 like the library (nanobind's default -Os makes the evaluation slower). FREE_THREADED: on a
# free-threaded Python the module keeps the GIL disabled (its shared state is thread-safe; see TASKS.md T6.6).
# STABLE_ABI: with Development.SABIModule the module uses only the stable ABI of Python 3.12, so that one wheel
# serves 3.12 and later (TASKS.md T6.7); nanobind ignores it before 3.12 and on a free-threaded Python.
nanobind_add_module(iohcpp NOMINSIZE FREE_THREADED STABLE_ABI ${SOURCES})
target_link_libraries(iohcpp PRIVATE ${PROJECT_NAME})
# the problem families of the registry (src/registry/families.hpp)
target_include_directories(iohcpp PRIVATE ${PROJECT_SOURCE_DIR}/src)

# The Python package (pyproject.toml installs only this component): the extension and the data files
install(TARGETS iohcpp LIBRARY DESTINATION ioh COMPONENT python)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/static/ DESTINATION ioh/static COMPONENT python)
