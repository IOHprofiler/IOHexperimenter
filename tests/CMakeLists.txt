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
# Remove files from the 'generators/' directory from being added to general test sources
file(GLOB_RECURSE ALL_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/cpp/*/*.cpp")
file(GLOB GENERATOR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/cpp/generators/*.cpp")
list(REMOVE_ITEM ALL_SOURCES ${GENERATOR_SOURCES})

# Remove MKLandscape if it is not enablecd
if(NOT ENABLE_MKLANDSCAPE_PROBLEMS)
    get_filename_component(MKLANDSCAPE_CPP ${CMAKE_CURRENT_SOURCE_DIR}/cpp/problem/test_mklandscape_problem.cpp ABSOLUTE)
    list(REMOVE_ITEM ALL_SOURCES "${MKLANDSCAPE_CPP}")
endif()

# Loop through each .cpp file in the generators/ directory
foreach(GENERATOR_SOURCE ${GENERATOR_SOURCES})
    # Extract the filename without the directory path and extension
    get_filename_component(GENERATOR_TARGET_NAME "${GENERATOR_SOURCE}" NAME_WE)

    # Define the executable for each source file
    add_executable(${GENERATOR_TARGET_NAME} ${GENERATOR_SOURCE})
    target_include_directories(${GENERATOR_TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/cpp)
    target_link_libraries(${GENERATOR_TARGET_NAME} PRIVATE ${PROJECT_NAME} gtest)
endforeach()

register_test(test_${PROJECT_NAME} "${ALL_SOURCES}")

# Add test for each file seperately
foreach(test_source ${ALL_SOURCES})
    get_filename_component(test_fname "${test_source}" NAME_WE)
    register_test(${test_fname} "${test_source}")
endforeach()
