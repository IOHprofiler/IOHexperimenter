## CMake build file for the ioh library test suite.
##
## This will build a Google Test test runner for all contained tests.
##

enable_testing()
add_subdirectory(${EXTERNAL_DIR}/googletest EXCLUDE_FROM_ALL build)
include(GoogleTest)

function(register_test test_name test_sources)
    add_executable(${test_name} ${test_sources} cpp/utils.hpp cpp/entrypoint.cpp)
    target_include_directories(${test_name} PRIVATE cpp)  
    if((${test_name} MATCHES test_mklandscape_problem) OR (${test_name}  MATCHES test_ioh) )
        if(ENABLE_MKLANDSCAPE_PROBLEMS)
        # link_directories(${EXTERNAL_DIR}/mklandscape/target/debug)
        #ToDo: Updating when problem_generator project is updated.
            target_link_libraries(${test_name} PRIVATE ${PROJECT_NAME} gtest problem_generator_lib)
        else()
            target_link_libraries(${test_name} PRIVATE ${PROJECT_NAME} gtest)
        endif()
    else()
        target_link_libraries(${test_name} PRIVATE ${PROJECT_NAME} gtest)
    endif()
    gtest_discover_tests(${test_name})
    target_include_directories(${test_name} PRIVATE cpp)
    target_link_libraries(${test_name} PRIVATE ${PROJECT_NAME} gtest)

    # When testing with ctest, only show messages up to the Warning.
    gtest_discover_tests(${test_name} EXTRA_ARGS "Warning" "'.*'" "99999")
endfunction()

# Add all tests in single executable
file(GLOB SOURCES cpp/*/*.cpp)

# Remove MKLandscape if it is not enablecd
if(NOT ENABLE_MKLANDSCAPE_PROBLEMS)
    get_filename_component(MKLANDSCAPE_CPP ${CMAKE_CURRENT_SOURCE_DIR}/cpp/problem/test_mklandscape_problem.cpp ABSOLUTE)
    list(REMOVE_ITEM SOURCES "${MKLANDSCAPE_CPP}")
endif()

register_test(test_${PROJECT_NAME} "${SOURCES}")

# Add test for each file seperately
foreach(test_source ${SOURCES}) 
    get_filename_component(test_fname "${test_source}" NAME_WE)
    register_test(${test_fname} "${test_source}")
endforeach()
