## CMake build file for the ioh library test suite.
##
## This will build a Google Test test runner for all contained tests.
##
add_subdirectory(vendor/googletest EXCLUDE_FROM_ALL)

add_executable(test_${PROJECT_NAME} "src/test_ioh.cpp")

target_include_directories(test_${PROJECT_NAME}
PRIVATE
    src
)

target_link_libraries(test_${PROJECT_NAME}
PRIVATE
    ${PROJECT_NAME} gtest gtest_main
)
