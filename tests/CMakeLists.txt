## CMake build file for the ioh library test suite.
##
## This builds one Google Test runner, test_ioh, with all tests. With IOH_TEST_PER_FILE, every test file also gets
## a runner of its own, and the programs that generate test fixtures are built.
##

add_subdirectory(${EXTERNAL_DIR}/googletest EXCLUDE_FROM_ALL build)
include(GoogleTest)

option(IOH_TEST_PER_FILE "Also build one test executable per test file, and the fixture generators" OFF)

function(register_test test_name test_sources)
    add_executable(${test_name} ${test_sources} cpp/utils.hpp cpp/entrypoint.cpp)
    target_include_directories(${test_name} PRIVATE cpp)
    # the golden fixtures (the registry test) and the data files of the tests
    target_compile_definitions(${test_name} PRIVATE IOH_GOLDEN_DIR="${PROJECT_SOURCE_DIR}/tests/data/golden"
        IOH_TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/tests/data")
    target_link_libraries(${test_name} PRIVATE ${PROJECT_NAME} gtest)
    if(ENABLE_MKLANDSCAPE_PROBLEMS AND ((${test_name} MATCHES test_mklandscape_problem) OR (${test_name} MATCHES test_ioh)))
        #ToDo: Updating when problem_generator project is updated.
        target_link_libraries(${test_name} PRIVATE problem_generator_lib)
    endif()
endfunction()

# Add all tests in single executable
# Remove files from the 'generators/' directory from being added to general test sources
file(GLOB_RECURSE ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/cpp/*/*.cpp")
file(GLOB GENERATOR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/cpp/generators/*.cpp")
list(REMOVE_ITEM ALL_SOURCES ${GENERATOR_SOURCES})

# Remove MKLandscape if it is not enablecd
if(NOT ENABLE_MKLANDSCAPE_PROBLEMS)
    get_filename_component(MKLANDSCAPE_CPP ${CMAKE_CURRENT_SOURCE_DIR}/cpp/problem/test_mklandscape_problem.cpp ABSOLUTE)
    list(REMOVE_ITEM ALL_SOURCES "${MKLANDSCAPE_CPP}")
endif()

register_test(test_${PROJECT_NAME} "${ALL_SOURCES}")

# ctest runs the tests of test_ioh, showing messages up to the Warning level. From CMake 3.18 on, it lists them
# when it runs, instead of running test_ioh after every build to list them.
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.18)
    gtest_discover_tests(test_${PROJECT_NAME} EXTRA_ARGS "Warning" "'.*'" "99999" DISCOVERY_MODE PRE_TEST)
else()
    gtest_discover_tests(test_${PROJECT_NAME} EXTRA_ARGS "Warning" "'.*'" "99999")
endif()

if(IOH_TEST_PER_FILE)
    # One test executable per file (ctest runs the tests once, from test_ioh)
    foreach(test_source ${ALL_SOURCES})
        get_filename_component(test_fname "${test_source}" NAME_WE)
        register_test(${test_fname} "${test_source}")
    endforeach()

    # Loop through each .cpp file in the generators/ directory
    foreach(GENERATOR_SOURCE ${GENERATOR_SOURCES})
        # Extract the filename without the directory path and extension
        get_filename_component(GENERATOR_TARGET_NAME "${GENERATOR_SOURCE}" NAME_WE)

        # Define the executable for each source file
        add_executable(${GENERATOR_TARGET_NAME} ${GENERATOR_SOURCE})
        target_include_directories(${GENERATOR_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/cpp)
        target_compile_definitions(${GENERATOR_TARGET_NAME} PRIVATE IOH_TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/tests/data")
        target_link_libraries(${GENERATOR_TARGET_NAME} PRIVATE ${PROJECT_NAME} gtest)
    endforeach()
endif()
