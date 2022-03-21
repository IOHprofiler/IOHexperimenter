add_executable(example_${PROJECT_NAME} "main.cpp")
target_link_libraries(example_${PROJECT_NAME} PRIVATE	${PROJECT_NAME})

add_executable(eafh "eafh.cpp")
target_link_libraries(eafh PRIVATE ioh)

add_executable(add_new_problem "add_new_problem.cpp")
target_link_libraries(add_new_problem PRIVATE ioh)

add_executable(gr "run_submodular_instances.cpp")
target_link_libraries(gr PRIVATE ioh)
