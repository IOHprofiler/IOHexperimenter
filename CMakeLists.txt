
######################################################################################
# Project settings
######################################################################################

cmake_minimum_required(VERSION 3.6)

enable_language(CXX) # C++

## Current version
set(VERSION_MAJOR 0 CACHE STRING "Major version number" )
set(VERSION_MINOR 0 CACHE STRING "Minor version number" )
set(VERSION_PATCH 0 CACHE STRING "Patch version number" )
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

# set the project name and version
project(IOHexperimenter VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(EXECUTABLE_OUTPUT_PATH "build/Cpp/")
# specify the CXX FLAGS
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    option(ENABLE_TESTS "Build tests binaries" ON)
endif()

######################################################################################
# Gather source files
######################################################################################

# set source file directorys and prepare for adding libraries.
set(PROBLEMS_BBOB_DIR "src/Problems/BBOB")
File(GLOB PROBLEMS_BBOB_SRC "${PROBLEMS_BBOB_DIR}/*.cpp" "${PROBLEMS_BBOB_DIR}/*.hpp"  "${PROBLEMS_BBOB_DIR}/*.h")

set(PROBLEMS_BBOB_COMMON_DIR "src/Problems/BBOB/bbob_common_used_functions")
File(GLOB PROBLEMS_BBOB_COMMON_SRC "${PROBLEMS_BBOB_COMMON_DIR}/*.cpp" "${PROBLEMS_BBOB_COMMON_DIR}/*.hpp" "${PROBLEMS_BBOB_COMMON_DIR}/*.h")

set(PROBLEMS_COMMON_DIR "src/Problems/common_used_functions")
File(GLOB PROBLEMS_COMMON_SRC "${PROBLEMS_COMMON_DIR}/*.cpp" "${PROBLEMS_COMMON_DIR}/*.hpp" "${PROBLEMS_COMMON_DIR}/*.h")

set(PROBLEMS_PBO_DIR "src/Problems/PBO")
File(GLOB PROBLEMS_PBO_SRC "${PROBLEMS_PBO_DIR}/*.cpp" "${PROBLEMS_PBO_DIR}/*.hpp" "${PROBLEMS_PBO_DIR}/*.h")

set(PROBLEMS_WMODEL_DIR "src/Problems/WModel")
File(GLOB PROBLEMS_WMODEL_SRC "${PROBLEMS_WMODEL_DIR}/*.cpp" "${PROBLEMS_WMODEL_DIR}/*.hpp" "${PROBLEMS_WMODEL_DIR}/*.h")

set(SUITES_DIR "src/Suites")
File(GLOB SUITES_SRC "${SUITES_DIR}/*.cpp" "${SUITES_DIR}/*.hpp" "${SUITES_DIR}/*.h")

set(TEMPLATE_DIR "src/Template")
File(GLOB TEMPLATE_SRC "${TEMPLATE_DIR}/*.cpp" "${TEMPLATE_DIR}/*.hpp" "${TEMPLATE_DIR}/*.h")

set(TEMPLATE_EXPERIMENTS_DIR "src/Template/Experiments")
File(GLOB TEMPLATE_EXPERIMENTS_SRC "${TEMPLATE_EXPERIMENTS_DIR}/*.cpp" "${TEMPLATE_EXPERIMENTS_DIR}/*.hpp" "${TEMPLATE_EXPERIMENTS_DIR}/*.h")

set(TEMPLATE_LOGGERS_DIR "src/Template/Loggers")
File(GLOB TEMPLATE_LOGGERS_SRC "${TEMPLATE_LOGGERS_DIR}/*.cpp" "${TEMPLATE_LOGGERS_DIR}/*.hpp" "${TEMPLATE_LOGGERS_DIR}/*.h")

set(IOHEXPERIMENTER_SRC 
  "${PROBLEMS_COMMON_SRC}"
  "${PROBLEMS_BBOB_SRC}"
  "${PROBLEMS_BBOB_COMMON_SRC}"
  "${PROBLEMS_PBO_SRC}"
  "${PROBLEMS_WMODEL_SRC}"
  "${SUITES_SRC}"
  "${TEMPLATE_SRC}"
  "${TEMPLATE_EXPERIMENTS_SRC}"
  "${TEMPLATE_LOGGERS_SRC}"
)

set(IOHEXPERIMENTER_DIR 
  "${PROBLEMS_COMMON_DIR}"
  "${PROBLEMS_BBOB_DIR}"
  "${PROBLEMS_BBOB_COMMON_DIR}"
  "${PROBLEMS_PBO_DIR}"
  "${PROBLEMS_WMODEL_DIR}"
  "${SUITES_DIR}"
  "${TEMPLATE_DIR}"
  "${TEMPLATE_EXPERIMENTS_DIR}"
  "${TEMPLATE_LOGGERS_DIR}"
)


######################################################################################
# Binaries
######################################################################################

# add the binary tree to the search path for include files
# so that we will find header files of IOHexperimenter.
include_directories(${IOHEXPERIMENTER_DIR})

# add the IOH library
add_library(IOH ${IOHEXPERIMENTER_SRC})

# add the executable
add_executable(IOHprofiler_run_experiment build/Cpp/IOHprofiler_run_experiment.cpp)
add_executable(IOHprofiler_run_problem build/Cpp/IOHprofiler_run_problem.cpp)
add_executable(IOHprofiler_run_suite build/Cpp/IOHprofiler_run_suite.cpp)

# link the IOH library to executable files.
target_link_libraries(IOHprofiler_run_experiment IOH)
target_link_libraries(IOHprofiler_run_problem    IOH)
target_link_libraries(IOHprofiler_run_suite      IOH)

if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

######################################################################################
# Installation
######################################################################################

# install. set name of the installed library as 'IOH'.
install(TARGETS IOH DESTINATION lib)
install(FILES ${IOHEXPERIMENTER_SRC} DESTINATION include)

