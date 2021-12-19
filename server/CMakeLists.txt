
include_directories(../include)

add_executable(ioh-server ioh-server.cpp)
target_link_libraries(ioh-server fmt)
