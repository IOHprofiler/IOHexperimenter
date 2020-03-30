# 
cmake_minimum_required(VERSION 3.6)

# set the project name and version
project(IOHexperimenter VERSION 1.0)
SET(EXECUTABLE_OUTPUT_PATH "build/Cpp/")

# add the executable
add_executable(IOHprofiler_run_experiment ./build/Cpp/IOHprofiler_run_experiment.cpp)
add_executable(IOHprofiler_run_problem ./build/Cpp/IOHprofiler_run_problem.cpp)
add_executable(IOHprofiler_run_suite ./build/Cpp/IOHprofiler_run_suite.cpp)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# specify the CXX FLAGS
set(CMAKE_CXX_FLAGS "-g -std=c++11 -Wall -Wno-unused-variable -Wno-sign-compare -Wno-unused-function -O2")

# set source file directorys and prepare for adding libraries.
SET(PROBLEMS_BBOB_DIR "src/Problems/BBOB")
FILE (GLOB PROBLEMS_BBOB_SRC "${PROBLEMS_BBOB_DIR}/*.hpp")
FILE (GLOB PROBLEMS_BBOB_INCLUDE "${PROBLEMS_BBOB_DIR}/*.hpp")

SET(PROBLEMS_BBOB_COMMON_DIR "src/Problems/BBOB/bbob_common_used_functions")
FILE (GLOB PROBLEMS_BBOB_COMMON_SRC "${PROBLEMS_BBOB_COMMON_DIR}/*.cpp" "${PROBLEMS_BBOB_COMMON_DIR}/*.hpp")
FILE (GLOB PROBLEMS_BBOB_COMMON_INCLUDE "${PROBLEMS_BBOB_COMMON_DIR}/*.hpp")

SET(PROBLEMS_COMMON_DIR "src/Problems/common_used_functions")
FILE (GLOB PROBLEMS_COMMON_SRC  "${PROBLEMS_COMMON_DIR}/*.hpp" )
FILE (GLOB PROBLEMS_COMMON_INCLUDE  "${PROBLEMS_COMMON_DIR}/*.hpp" )

SET(PROBLEMS_PBO_DIR "src/Problems/PBO")
FILE (GLOB PROBLEMS_PBO_SRC "${PROBLEMS_PBO_DIR}/*.hpp" )
FILE (GLOB PROBLEMS_PBO_INCLUDE "${PROBLEMS_PBO_DIR}/*.hpp" )

SET(PROBLEMS_WMODEL_DIR "src/Problems/WModel")
FILE (GLOB PROBLEMS_WMODEL_SRC  "${PROBLEMS_WMODEL_DIR}/*.hpp")
FILE (GLOB PROBLEMS_WMODEL_INCLUDE  "${PROBLEMS_WMODEL_DIR}/*.hpp")

SET(SUITES_DIR "src/Suites")
FILE (GLOB SUITES_SRC "${SUITES_DIR}/*.h" "${SUITES_DIR}/*.hpp")
FILE (GLOB SUITES_INCLUDE "${SUITES_DIR}/*.h" "${SUITES_DIR}/*.hpp")

SET(TEMPLATE_DIR "src/Template")
FILE (GLOB TEMPLATE_SRC "${TEMPLATE_DIR}/*.cpp" "${TEMPLATE_DIR}/*.hpp" "${TEMPLATE_DIR}/*.h") 
#aux_source_directory(src/Template/ DIR_TEMP)
FILE (GLOB TEMPLATE_INCLUDE "${TEMPLATE_DIR}/*.h" "${TEMPLATE_DIR}/*.hpp") 

SET(TEMPLATE_EXPERIMENTS_DIR "src/Template/Experiments")
FILE (GLOB TEMPLATE_EXPERIMENTS_SRC "${TEMPLATE_EXPERIMENTS_DIR}/*.hpp" "${TEMPLATE_EXPERIMENTS_DIR}/*.cpp")
FILE (GLOB TEMPLATE_EXPERIMENTS_INCLUDE "${TEMPLATE_EXPERIMENTS_DIR}/*.hpp")

SET(TEMPLATE_LOGGERS_DIR "src/Template/Loggers")
FILE (GLOB TEMPLATE_LOGGERS_SRC "${TEMPLATE_LOGGERS_DIR}/*.cpp" "${TEMPLATE_LOGGERS_DIR}/*.hpp")
FILE (GLOB TEMPLATE_LOGGERS_INCLUDE "${TEMPLATE_LOGGERS_DIR}/*.hpp")

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

SET(IOHEXPERIMENTER_INCLUDE 
  "${PROBLEMS_COMMON_INCLUDE}"
  "${PROBLEMS_BBOB_INCLUDE}"
  "${PROBLEMS_BBOB_COMMON_INCLUDE}"
  "${PROBLEMS_PBO_INCLUDE}"
  "${PROBLEMS_WMODEL_INCLUDE}"
  "${SUITES_INCLUDE}"
  "${TEMPLATE_INCLUDE}"
  "${TEMPLATE_EXPERIMENTS_INCLUDE}"
  "${TEMPLATE_LOGGERS_INCLUDE}"
)


# add the binary tree to the search path for include files
# so that we will find header files of IOHexperimenter.
include_directories(${IOHEXPERIMENTER_DIR})

# add the IOH library
add_library(IOH ${IOHEXPERIMENTER_SRC})

# link the IOH library to executable files.
target_link_libraries(IOHprofiler_run_experiment IOH )
target_link_libraries(IOHprofiler_run_problem IOH)
target_link_libraries(IOHprofiler_run_suite IOH)

# install. set name of the installed library as 'IOH'.
install (TARGETS IOH DESTINATION lib)
install (FILES ${IOHEXPERIMENTER_INCLUDE} DESTINATION include)
