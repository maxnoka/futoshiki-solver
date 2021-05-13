# Works with 3.11 and tested through 3.20
cmake_minimum_required(VERSION 3.11...3.20)

project(
  Futoshiki
  VERSION 0.1
  DESCRIPTION "A Futoshiki solver and puzzle generator"
  LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_EXTENSIONS OFF)

  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  include(CTest)
  include(FetchContent)
endif()

set(BUILD_EXAMPLES OFF CACHE BOOL "Builds the examples in the project")
set(BUILD_TESTING OFF CACHE BOOL "Builds the tests in the project")
add_subdirectory(extern/crow)

# The compiled library code is here
add_subdirectory(src)

# The executable code is here
add_subdirectory(cli-app)

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING)
   AND BUILD_TESTING)
  add_subdirectory(tests)
endif()
