## CMake build file for the ioh library test suite.
##
## This will build a Google Test test runner for all contained tests.
##
add_subdirectory(${EXTERNAL_DIR}/googletest EXCLUDE_FROM_ALL build)

file(GLOB_RECURSE SOURCES src *.cpp) 

add_executable(test_${PROJECT_NAME} ${SOURCES})

target_include_directories(test_${PROJECT_NAME}
PRIVATE
    src
)

target_link_libraries(test_${PROJECT_NAME}
PRIVATE
    ${PROJECT_NAME} gtest
)
  