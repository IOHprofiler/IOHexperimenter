# 
cmake_minimum_required(VERSION 3.6)

# set the project name and version
project(IOHexperimenter VERSION 1.0)
SET(EXECUTABLE_OUTPUT_PATH "build/Cpp/")

# add the executable
add_executable(IOHprofiler_run_experiment build/Cpp/IOHprofiler_run_experiment.cpp)
add_executable(IOHprofiler_run_problem build/Cpp/IOHprofiler_run_problem.cpp)
add_executable(IOHprofiler_run_suite build/Cpp/IOHprofiler_run_suite.cpp)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# specify the CXX FLAGS
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    option(ENABLE_TESTS "Build tests binaries" ON)
endif()


######################################################################################
# Gather source files
######################################################################################

# set source file directorys and prepare for adding libraries.
SET(PROBLEMS_BBOB_DIR "src/Problems/BBOB")
FILE (GLOB PROBLEMS_BBOB_SRC "${PROBLEMS_BBOB_DIR}/*.cpp" "${PROBLEMS_BBOB_DIR}/*.hpp"  "${PROBLEMS_BBOB_DIR}/*.h")

SET(PROBLEMS_BBOB_COMMON_DIR "src/Problems/BBOB/bbob_common_used_functions")
FILE (GLOB PROBLEMS_BBOB_COMMON_SRC "${PROBLEMS_BBOB_COMMON_DIR}/*.cpp" "${PROBLEMS_BBOB_COMMON_DIR}/*.hpp" "${PROBLEMS_BBOB_COMMON_DIR}/*.h")

SET(PROBLEMS_COMMON_DIR "src/Problems/common_used_functions")
FILE (GLOB PROBLEMS_COMMON_SRC "${PROBLEMS_COMMON_DIR}/*.cpp" "${PROBLEMS_COMMON_DIR}/*.hpp" "${PROBLEMS_COMMON_DIR}/*.h")

SET(PROBLEMS_PBO_DIR "src/Problems/PBO")
FILE (GLOB PROBLEMS_PBO_SRC "${PROBLEMS_PBO_DIR}/*.cpp" "${PROBLEMS_PBO_DIR}/*.hpp" "${PROBLEMS_PBO_DIR}/*.h")

SET(PROBLEMS_WMODEL_DIR "src/Problems/WModel")
FILE (GLOB PROBLEMS_WMODEL_SRC "${PROBLEMS_WMODEL_DIR}/*.cpp" "${PROBLEMS_WMODEL_DIR}/*.hpp" "${PROBLEMS_WMODEL_DIR}/*.h")

SET(SUITES_DIR "src/Suites")
FILE (GLOB SUITES_SRC "${SUITES_DIR}/*.cpp" "${SUITES_DIR}/*.hpp" "${SUITES_DIR}/*.h")

SET(TEMPLATE_DIR "src/Template")
FILE (GLOB TEMPLATE_SRC "${TEMPLATE_DIR}/*.cpp" "${TEMPLATE_DIR}/*.hpp" "${TEMPLATE_DIR}/*.h")

SET(TEMPLATE_EXPERIMENTS_DIR "src/Template/Experiments")
FILE (GLOB TEMPLATE_EXPERIMENTS_SRC "${TEMPLATE_EXPERIMENTS_DIR}/*.cpp" "${TEMPLATE_EXPERIMENTS_DIR}/*.hpp" "${TEMPLATE_EXPERIMENTS_DIR}/*.h")

SET(TEMPLATE_LOGGERS_DIR "src/Template/Loggers")
FILE (GLOB TEMPLATE_LOGGERS_SRC "${TEMPLATE_LOGGERS_DIR}/*.cpp" "${TEMPLATE_LOGGERS_DIR}/*.hpp" "${TEMPLATE_LOGGERS_DIR}/*.h")

SET(IOHEXPERIMENTER_SRC 
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

SET(IOHEXPERIMENTER_DIR 
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

# add the binary tree to the search path for include files
# so that we will find header files of IOHexperimenter.
include_directories(${IOHEXPERIMENTER_DIR})

# add the IOH library
add_library(IOH ${IOHEXPERIMENTER_SRC})

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
install (TARGETS IOH DESTINATION lib)
install (FILES ${IOHEXPERIMENTER_SRC} DESTINATION include)
