## CMake build file for the ioh library test suite.
##
## This will build a Google Test test runner for all contained tests.
##
add_subdirectory(${EXTERNAL_DIR}/googletest EXCLUDE_FROM_ALL build)
include(GoogleTest)

enable_testing()

function(register_test test_name test_sources)
    add_executable(test_${test_name} ${test_sources} cpp/utils.hpp cpp/entrypoint.cpp)

    target_include_directories(test_${test_name}
    PRIVATE
        cpp
    )
    target_link_libraries(test_${test_name}
    PRIVATE
        ${PROJECT_NAME} gtest
    )
    gtest_discover_tests(test_${test_name})
endfunction()

# Add all tests
file(GLOB_RECURSE SOURCES cpp *.cpp *.hpp)  
register_test(${PROJECT_NAME} "${SOURCES}" true)

register_test(common cpp/common/test_common.cpp)
register_test(experimenter cpp/experimenter/test_experiment.cpp) 
register_test(logger_default cpp/logger/test_default.cpp)
register_test(logger_ecdf cpp/logger/test_ecdf.cpp)
register_test(logger_observer cpp/logger/test_observer.cpp)
register_test(problems_generic cpp/problem/test_problems.cpp)
register_test(problems_bbob cpp/problem/test_bbob_problem.cpp)
register_test(problems_pbo cpp/problem/test_pbo_problem.cpp)
register_test(problems_wmodel cpp/problem/test_wmodel_problem.cpp)
register_test(suite_generic cpp/suite/test_suite.cpp)