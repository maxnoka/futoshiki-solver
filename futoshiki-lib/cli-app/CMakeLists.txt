add_executable(cli_app main.cpp)

target_compile_features(cli_app PRIVATE cxx_std_17)
target_link_libraries(cli_app PRIVATE z) 
target_link_libraries(cli_app PRIVATE futoshiki_library) 
