## CMake build file for the ioh library test suite.
##
## This will build a Google Test test runner for all contained tests.
##
add_subdirectory(${EXTERNAL_DIR}/googletest EXCLUDE_FROM_ALL build)

file(GLOB_RECURSE SOURCES cpp *.cpp *.hpp)  

add_executable(test_${PROJECT_NAME} ${SOURCES})

target_include_directories(test_${PROJECT_NAME}
PRIVATE
    cpp
)
target_link_libraries(test_${PROJECT_NAME}
PRIVATE
    ${PROJECT_NAME} gtest
)

add_test(test_${PROJECT_NAME} test_${PROJECT_NAME})
  