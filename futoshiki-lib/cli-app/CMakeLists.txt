add_executable(cli_app main.cpp)

target_link_libraries(cli_app PRIVATE z) 
target_link_libraries(cli_app PRIVATE futoshiki_library) 
