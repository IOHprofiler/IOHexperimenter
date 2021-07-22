add_executable(example_${PROJECT_NAME} "main.cpp")

target_link_libraries(example_${PROJECT_NAME}
PRIVATE
	${PROJECT_NAME}
)

add_executable(eafh "eafh.cpp")
target_link_libraries(eafh PRIVATE ioh)
