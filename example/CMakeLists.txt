#add_executable(example_${PROJECT_NAME} "main.cpp")
#target_link_libraries(example_${PROJECT_NAME} PRIVATE	${PROJECT_NAME})

#add_executable(eafh "eafh.cpp")
#target_link_libraries(eafh PRIVATE ioh)


add_executable(analyzer "analyzer.cpp")
target_link_libraries(analyzer PRIVATE ioh)
