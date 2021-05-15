set( WEB_SOURCES_LIST 
  "${Futoshiki_SOURCE_DIR}/web-app/main.cpp"
  "${Futoshiki_SOURCE_DIR}/web-app/EasyLoggingCrowLogHandler.cpp"
  "${Futoshiki_SOURCE_DIR}/web-app/EasyLoggingCrowLogHandler.hpp"
)
add_executable(web_app ${WEB_SOURCES_LIST})

target_link_libraries(web_app PRIVATE crow_library z futoshiki_library) 
