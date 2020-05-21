
function(add_simple_test tname)
    add_executable(${tname} ${tname}.cpp)
    target_link_libraries(${tname} IOH)
    add_test(NAME ${tname} COMMAND ${tname})
endfunction()

file(GLOB sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

list(FILTER sources EXCLUDE REGEX ".*python.*")

foreach(filename ${sources})
        # File name without directory or longest extension
        get_filename_component(name ${filename} NAME_WE)
        add_simple_test(${name})
endforeach()

if(ENABLE_PYTHON_PROBLEMS AND PYTHONLIBS_FOUND)
    add_executable(t-python-problem t-python-problem.cpp)
    target_link_libraries(t-python-problem IOH ${PYTHON_LIBRARIES})
    add_test(NAME t-python-problem COMMAND t-python-problem test_problem.py call)
endif()

