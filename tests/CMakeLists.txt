
function(add_simple_test tname)
    add_executable(${tname} ${tname}.cpp)
    target_link_libraries(${tname} IOH)
    add_test(NAME ${tname} COMMAND ${tname})
endfunction()

file(GLOB sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

foreach(filename ${sources})
        # File name without directory or longest extension
        get_filename_component(name ${filename} NAME_WE)
        add_simple_test(${name})
endforeach()

