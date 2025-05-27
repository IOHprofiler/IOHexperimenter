add_subdirectory(${EXTERNAL_DIR}/pybind11 build)

set(SOURCES 
    src/ioh.cpp 
    src/problem_helpers.cpp 
    src/logger.cpp 
    src/trigger.cpp 
    src/property.cpp 
    src/empirical_attainment.cpp 
    src/empirical_attainment.cpp 
    src/problem.cpp 
    src/real.cpp
    src/integer.cpp
    src/suite.cpp
    src/real_suite.cpp
    src/integer_suite.cpp
    )
pybind11_add_module(iohcpp ${SOURCES})

target_link_libraries(iohcpp PRIVATE ${PROJECT_NAME})  
target_compile_definitions(iohcpp PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
